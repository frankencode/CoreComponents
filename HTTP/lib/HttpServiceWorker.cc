/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/HttpServiceWorkerState>
#include <cc/httpDebug>
#include <cc/HttpError>
#include <cc/StreamTap>
#include <cc/TapBuffer>
#include <cc/ScopeGuard>
#include <cc/Casefree>
#include <cc/DEBUG>

namespace cc {

HttpServiceWorkerState::HttpServiceWorkerState(
    const HttpServerConfig &nodeConfig,
    const PendingConnections &pendingConnections,
    const ClosedConnections &closedConnections,
    const TlsSessionCache &sessionCache
):
    nodeConfig_{nodeConfig},
    pendingConnections_{pendingConnections},
    closedConnections_{closedConnections},
    sessionCache_{sessionCache}
{}

HttpServiceWorkerState::~HttpServiceWorkerState()
{
    pendingConnections_.close();
    if (thread_) thread_.wait();
}

void HttpServiceWorkerState::start()
{
    thread_ = Thread{[this]{ run(); }};
    thread_.start();
}

void HttpServiceWorkerState::run()
{
    while (true) {
        try {
            if (!pendingConnections_.popFront(&client_)) break;

            try {
                Stream stream = client_.stream();
                initiate(stream);
                serve(stream);
            }
            catch (Exception &ex) {
                #ifndef NDEBUG
                CCNODE_DEBUG() << ex.message() << nl;
                #endif
            }
            catch (HttpCloseRequest &)
            {}

            closeConnection();

            serviceDelegate_ = HttpServiceDelegate{};
            serviceInstance_ = HttpServiceInstance{};
        }
        catch (IoExhaustion &)
        {}
        catch (Exception &ex) {
            CCNODE_DEBUG() << ex << nl;
        }
    }
}

void HttpServiceWorkerState::setupTap(Stream &stream)
{
    if (errorLoggingInstance().verbosity() >=  LoggingLevel::Debug) {
        String label = client_.peerAddress().toString();
        Stream log = errorLoggingInstance().debugStream();
        TapBuffer inputBuffer{log, label + " >> "};
        TapBuffer outputBuffer{log, label + " << "};
        stream = StreamTap{stream, inputBuffer, outputBuffer};
    }
}

void HttpServiceWorkerState::initiate(Stream &stream)
{
    if (
        nodeConfig_.forceSecureTransport() ||
        client_.localAddress().port() == 443 ||
        client_.localAddress().port() == 4443
    ) {
        TlsServerStream tlsStream{client_.stream(), client_.peerAddress(), nodeConfig_.tlsOptions(), sessionCache_};
        tlsStream.handshake(
            [this](const String &serverName, const SocketAddress &peerAddress){
                serviceInstance_ = nodeConfig_.selectService(serverName);
                return serviceInstance_.tlsOptions();
            },
            nodeConfig_.connectionTimeoutMs()
        );
        stream = tlsStream;
        setupTap(stream);
        requestParser_ = HttpRequestParser{tlsStream};
    }
    else {
        setupTap(stream);
        requestParser_ = HttpRequestParser{stream};
        HttpRequest request = readRequest();
        serviceInstance_ = nodeConfig_.selectService(request.host(), request.uri());
        pendingRequest_ = request;
    }

    if (!serviceInstance_) throw HttpBadRequest{};

    serviceDelegate_ = serviceInstance_.createDelegate();
    serviceDelegate_->workerState_ = this;
}

void HttpServiceWorkerState::serve(Stream &stream)
{
    HttpRequest request;
    int requestCount = 0;

    try {
        while (client_)
        {
            // CCNODE_DEBUG() << "Reading request..." << nl;

            request = readRequest();
            ++requestCount;

            {
                ScopeGuard guard{[this]{ response_ = HttpResponseGenerator{}; }};

                response_ = HttpResponseGenerator{stream};
                response_.setNodeVersion(nodeConfig_.version());

                serviceDelegate_.process(request);

                response_.endTransmission();

                if (response_.delivered()) {
                    accessLoggingInstance().logDelivery(client_, request, response_.status(), response_.bytesWritten());
                    if (!requestParser_.isPayloadConsumed())
                        break;
                }
                else break;
            }
            if (
                Casefree{request.header("Connection")} == "close" ||
                requestCount >= serviceInstance_.requestLimit() ||
                (request.majorVersion() == 1 && request.minorVersion() == 0)
            ) break;
        }
    }
    catch (HttpError &error)
    {
        if (requestCount > 0 || error.status() == HttpStatus::RequestTimeout) {
            try {
                Format{"HTTP/1.1 %% %%\r\n\r\n", stream}
                    << +error.status()
                    << error.message();
            }
            catch(...)
            {}
            accessLoggingInstance().logDelivery(client_, request, error.status());
        }
    }
}

HttpRequest HttpServiceWorkerState::readRequest()
{
    HttpRequest request;
    if (pendingRequest_) {
        request = pendingRequest_;
        pendingRequest_ = HttpRequest{};
    }
    else {
        request = requestParser_.readRequest();
    }
    return request;
}

void HttpServiceWorkerState::closeConnection()
{
    if (client_) {
        // CCNODE_DEBUG() << "Closing connection to " << client_.peerAddress() << nl;
        client_.updateDepartureTime();
        closedConnections_.pushBack(client_);
        client_ = HttpClientConnection{};
    }
}

HttpServiceWorker::HttpServiceWorker(
    const HttpServerConfig &nodeConfig,
    const PendingConnections &pendingConnections,
    const ClosedConnections &closedConnections,
    const TlsSessionCache &sessionCache
):
    Object{
        new State{
            nodeConfig,
            pendingConnections,
            closedConnections,
            sessionCache
        }
    }
{}

void HttpServiceWorker::start()
{
    me().start();
}

const HttpServerConfig &HttpServiceWorker::nodeConfig() const
{
    return me().nodeConfig_;
}

const HttpServiceInstance &HttpServiceWorker::serviceInstance() const
{
    return me().serviceInstance_;
}

HttpClientConnection &HttpServiceWorker::client()
{
    return me().client_;
}

void HttpServiceWorker::closeConnection()
{
    return me().closeConnection();
}

HttpResponseGenerator &HttpServiceWorker::response()
{
    return me().response_;
}

HttpServiceWorker::State &HttpServiceWorker::me()
{
    return Object::me.as<State>();
}

const HttpServiceWorker::State &HttpServiceWorker::me() const
{
    return Object::me.as<State>();
}

} //
