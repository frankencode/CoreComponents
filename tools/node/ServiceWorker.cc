/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Format>
#include <cc/RefGuard>
#include <cc/System>
#include <cc/net/Uri>
#include <cc/http/exceptions>
#include "ErrorLog.h"
#include "AccessLog.h"
#include "NodeConfig.h"
#include "ServiceDefinition.h"
#include "ServiceDelegate.h"
#include "HttpResponseGenerator.h"
#include "ServiceWorker.h"

namespace ccnode {

using namespace cc::http;

Ref<ServiceWorker> ServiceWorker::create(PendingConnections *pendingConnections, ClosedConnections *closedConnections)
{
    return new ServiceWorker(pendingConnections, closedConnections);
}

ServiceWorker::ServiceWorker(PendingConnections *pendingConnections, ClosedConnections *closedConnections):
    pendingConnections_(pendingConnections),
    closedConnections_(closedConnections)
{}

ServiceWorker::~ServiceWorker()
{
    pendingConnections_->push(Ref<HttpServerConnection>());
    Thread::wait();
}

void ServiceWorker::logDelivery(HttpServerConnection *client, int statusCode, size_t bytesWritten, const String &statusMessage)
{
    Stream *stream = accessLog()->noticeStream();
    if (400 <= statusCode && statusCode <= 499) stream = accessLog()->debugStream();
    else if (500 <= statusCode) stream = accessLog()->errorStream();

    HttpRequest *request = client->request();
    String requestHost = request ? request->host() : "";
    String requestLine = request ? request->line() : "";
    double requestTime = request ? request->time() : System::now();
    String userAgent   = request ? request->value("User-Agent") : statusMessage;

    Format(stream)
        << client->address()->networkAddress() << " "
        << Date::breakdown(requestTime)->toString() << " "
        << "\"" << requestHost << "\" "
        << "\"" << requestLine << "\" "
        << statusCode << " " << bytesWritten << " "
        << "\"" << userAgent << "\" "
        << client->priority()
        << nl;
}

void ServiceWorker::run()
{
    errorLog()->open(nodeConfig()->errorLogConfig());
    accessLog()->open(nodeConfig()->accessLogConfig());

    while (true) {
        try {
            if (!pendingConnections_->popFront(&client_)) break;

            int requestCount = 0;
            try {
                serviceInstance_ = client_->handshake();
                if (!serviceInstance_) throw BadRequest();

                serviceDelegate_ = serviceInstance_->createDelegate(this);

                while (client_) {
                    CCNODE_DEBUG() << "Reading request..." << nl;
                    Ref<HttpRequest> request = client_->readRequest();
                    ++requestCount;
                    {
                        RefGuard<HttpResponseGenerator> guard(&response_);
                        response_ = HttpResponseGenerator::create(client_);
                        response_->setNodeVersion(nodeConfig()->version());

                        serviceDelegate_->process(request);
                        response_->endTransmission();
                        if (response_->delivered()) {
                            logDelivery(client_, response_->statusCode(), response_->bytesWritten());
                            if (!client_->isPayloadConsumed())
                                break;
                        }
                        else break;
                    }
                    if (
                        request->value("Connection")->equalsCaseInsensitive("close") ||
                        requestCount >= serviceInstance_->requestLimit() ||
                        (request->majorVersion() == 1 && request->minorVersion() == 0)
                    ) break;
                }
            }
            catch (ProtocolException &ex) {
                if (requestCount > 0 || ex.statusCode() == RequestTimeout::StatusCode) {
                    try {
                        Format("HTTP/1.1 %% %%\r\n\r\n", client_->stream()) << ex.statusCode() << " " << ex.message();
                    }
                    catch(Exception &)
                    {}
                    catch(...)
                    {}
                    logDelivery(client_, ex.statusCode());
                }
            }
            catch (Exception &ex) {
                CCNODE_ERROR() << ex.message() << nl;
            }
            catch (CloseRequest &)
            {}

            closeConnection();

            serviceDelegate_ = 0;
            serviceInstance_ = 0;
        }
        catch (ConnectionResetByPeer &)
        {}
        catch (Exception &ex) {
            CCNODE_ERROR() << ex.message() << nl;
        }
    }
}

HttpResponseGenerator *ServiceWorker::response() const
{
    return response_;
}

void ServiceWorker::autoSecureForwardings()
{
    if (response()->statusCode() == 302) {
        HttpResponseGenerator::Header *header = response()->header();
        if (
            nodeConfig()->security()->hasCredentials() ||
            serviceInstance_->security()->hasCredentials()
        ) {
            String location = header->value("Location");
            if (location->startsWith("http:")) {
                try {
                    Ref<Uri> uri = Uri::parse(location);
                    if (serviceInstance_->host()->match(uri->host())) {
                        uri->setScheme("https");
                        if (uri->port() > 0)
                            uri->setPort(nodeConfig()->securePort());
                        header->establish("Location", uri->toString());
                    }
                }
                catch (...)
                {}
            }
        }
    }
}

void ServiceWorker::closeConnection()
{
    if (client_) {
        CCNODE_DEBUG() << "Closing connection to " << client_->address() << nl;
        Ref<ConnectionInfo> visit = client_->connectionInfo();
        visit->updateDepartureTime();
        client_ = 0;
        closedConnections_->push(visit);
    }
}

} // namespace ccnode
