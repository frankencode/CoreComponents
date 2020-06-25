/*
 * Copyright (C) 2007-2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/http/DeliveryWorker>
#include <cc/http/NodeConfig>
#include <cc/http/DeliveryService>
#include <cc/http/DeliveryDelegate>
#include <cc/http/HttpResponseGenerator>
#include <cc/http/exceptions>
#include <cc/http/debug>
#include <cc/net/Uri>
#include <cc/Format>
#include <cc/RefGuard>

namespace cc {
namespace http {

Ref<DeliveryWorker> DeliveryWorker::create(const NodeConfig *nodeConfig, const PendingConnections &pendingConnections, const ClosedConnections &closedConnections)
{
    return new DeliveryWorker{nodeConfig, pendingConnections, closedConnections};
}

DeliveryWorker::DeliveryWorker(const NodeConfig *nodeConfig, const PendingConnections &pendingConnections, const ClosedConnections &closedConnections):
    nodeConfig_{nodeConfig},
    pendingConnections_{pendingConnections},
    closedConnections_{closedConnections}
{}

DeliveryWorker::~DeliveryWorker()
{
    pendingConnections_->pushBack(Ref<HttpServerConnection>{});
    Thread::wait();
}

const LoggingInstance *DeliveryWorker::errorLoggingInstance() const
{
    return deliveryInstance_ ? deliveryInstance_->errorLoggingInstance() : nodeConfig_->errorLoggingInstance();
}

const LoggingInstance *DeliveryWorker::accessLoggingInstance() const
{
    return deliveryInstance_ ? deliveryInstance_->accessLoggingInstance() : nodeConfig_->accessLoggingInstance();
}

void DeliveryWorker::run()
{
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
                            accessLoggingInstance()->logDelivery(client_, response_->statusCode(), response_->bytesWritten());
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
            catch (HttpError &ex) {
                if (requestCount > 0 || ex.statusCode() == RequestTimeout::StatusCode) {
                    try {
                        Format{"HTTP/1.1 %% %%\r\n\r\n", client_->stream()}
                            << ex.statusCode()
                            << ex.message();
                    }
                    catch(...)
                    {}
                    accessLoggingInstance()->logDelivery(client_, ex.statusCode());
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
                    Uri uri{location};
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
        closedConnections_->write(visit);
    }
}

}} // namespace cc::http
