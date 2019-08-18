/*
 * Copyright (C) 2007-2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/node/DeliveryWorker>
#include <cc/node/ErrorLog>
#include <cc/node/AccessLog>
#include <cc/node/NodeConfig>
#include <cc/node/DeliveryService>
#include <cc/node/DeliveryDelegate>
#include <cc/node/HttpResponseGenerator>
#include <cc/node/exceptions>
#include <cc/net/Uri>
#include <cc/Format>
#include <cc/RefGuard>
#include <cc/System>
#include <cc/Date>

namespace cc {
namespace node {

Ref<DeliveryWorker> DeliveryWorker::create(const NodeConfig *nodeConfig, PendingConnections *pendingConnections, ClosedConnections *closedConnections)
{
    return new DeliveryWorker{nodeConfig, pendingConnections, closedConnections};
}

DeliveryWorker::DeliveryWorker(const NodeConfig *nodeConfig, PendingConnections *pendingConnections, ClosedConnections *closedConnections):
    nodeConfig_{nodeConfig},
    pendingConnections_{pendingConnections},
    closedConnections_{closedConnections}
{}

DeliveryWorker::~DeliveryWorker()
{
    pendingConnections_->push(Ref<HttpServerConnection>{});
    Thread::wait();
}

void DeliveryWorker::logDelivery(HttpServerConnection *client, int statusCode, size_t bytesWritten, const String &statusMessage)
{
    Stream *stream = AccessLog::instance()->noticeStream();
    if (400 <= statusCode && statusCode <= 499) stream = AccessLog::instance()->debugStream();
    else if (500 <= statusCode) stream = AccessLog::instance()->errorStream();

    HttpRequest *request = client->request();
    String requestHost = request ? request->host() : "";
    String requestLine = request ? request->line() : "";
    double requestTime = request ? request->time() : System::now();
    String userAgent   = request ? request->value("User-Agent") : statusMessage;

    Format{stream}
        << client->address()->networkAddress() << " "
        << Date::breakdown(requestTime)->toString() << " "
        << "\"" << requestHost << "\" "
        << "\"" << requestLine << "\" "
        << statusCode << " " << bytesWritten << " "
        << "\"" << userAgent << "\" "
        << client->priority()
        << nl;
}

void DeliveryWorker::run()
{
    ErrorLog::instance()->open(nodeConfig()->errorLogConfig());
    AccessLog::instance()->open(nodeConfig()->accessLogConfig());

    while (true) {
        try {
            if (!pendingConnections_->popFront(&client_)) break;

            int requestCount = 0;
            try {
                deliveryInstance_ = client_->handshake();
                if (!deliveryInstance_) throw BadRequest{};

                deliveryDelegate_ = deliveryInstance_->createDelegate(this);

                while (client_) {
                    // CCNODE_DEBUG() << "Reading request..." << nl;
                    Ref<HttpRequest> request = client_->readRequest();
                    ++requestCount;
                    {
                        RefGuard<HttpResponseGenerator> guard{&response_};
                        response_ = HttpResponseGenerator::create(client_);
                        response_->setNodeVersion(nodeConfig()->version());

                        deliveryDelegate_->process(request);
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
                        requestCount >= deliveryInstance_->requestLimit() ||
                        (request->majorVersion() == 1 && request->minorVersion() == 0)
                    ) break;
                }
            }
            catch (ProtocolException &ex) {
                if (requestCount > 0 || ex.statusCode() == RequestTimeout::StatusCode) {
                    try {
                        Format("HTTP/1.1 %% %%\r\n\r\n", client_->stream())
                            << ex.statusCode()
                            << ex.message();
                    }
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

            deliveryDelegate_ = nullptr;
            deliveryInstance_ = nullptr;
        }
        catch (ConnectionResetByPeer &)
        {}
        catch (Exception &ex) {
            CCNODE_ERROR() << ex << nl;
        }
    }
}

HttpResponseGenerator *DeliveryWorker::response() const
{
    return response_;
}

void DeliveryWorker::autoSecureForwardings()
{
    if (response()->statusCode() == 302) {
        HttpResponseGenerator::Header *header = response()->header();
        if (
            nodeConfig()->security()->hasCredentials() ||
            deliveryInstance_->security()->hasCredentials()
        ) {
            String location = header->value("Location");
            if (location->startsWith("http:")) {
                try {
                    Ref<Uri> uri = Uri::parse(location);
                    if (deliveryInstance_->host()->match(uri->host())) {
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

void DeliveryWorker::closeConnection()
{
    if (client_) {
        // CCNODE_DEBUG() << "Closing connection to " << client_->address() << nl;
        Ref<ConnectionInfo> visit = client_->connectionInfo();
        visit->updateDepartureTime();
        client_ = nullptr;
        closedConnections_->push(visit);
    }
}

}} // namespace cc::node
