/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/System>
#include <cc/LineSource>
#include "exceptions.h"
#include "ErrorLog.h"
#include "NodeConfig.h"
#include "HttpClientConnection.h"

namespace ccnode {

using namespace cc;

Ref<HttpClientConnection> HttpClientConnection::open(HttpClientSocket *socket)
{
    return new HttpClientConnection(socket);
}

HttpClientConnection::HttpClientConnection(HttpClientSocket *socket):
    HttpConnection(socket),
    socket_(socket),
    connectionInfo_(ConnectionInfo::create(socket->address()))
{
    if (errorLog()->level() >= DebugLogLevel)
        setupTransferLog(errorLog()->debugStream(), socket->address()->toString());
}

ServiceInstance *HttpClientConnection::handshake()
{
    ServiceInstance *serviceInstance = 0;

    if (socket_->isSecure()) {
        serviceInstance = socket_->handshake();
    }
    else {
        String host, uri;
        Ref<HttpRequest> request = readRequest();
        host = request->host();
        uri = request->uri();
        putBack(request);

        serviceInstance = nodeConfig()->selectService(host, uri);
    }

    return serviceInstance;
}

bool HttpClientConnection::isSecure() const
{
    return socket_->isSecure();
}

Ref<HttpRequest> HttpClientConnection::readRequest()
{
    if (pendingRequest_) {
        Ref<HttpRequest> h = pendingRequest_;
        pendingRequest_ = 0;
        return h;
    }
    request_ = HttpRequest::create();
    request_->time_ = System::now();
    readMessage(request_);
    return request_;
}

void HttpClientConnection::putBack(HttpRequest *request)
{
    pendingRequest_ = request;
}

void HttpClientConnection::readFirstLine(LineSource *source, HttpMessage *message)
{
    Ref<HttpRequest> request = message;

    String line;
    if (!source->read(&line)) throw CloseRequest();
    request->line_ = line;

    if (line->count(' ') != 2) throw BadRequest();

    int i0 = 0, i1 = line->find(' ');
    request->method_ = line->copy(i0, i1);
    i0 = i1 + 1; i1 = line->find(' ', i0);
    request->uri_ = line->copy(i0, i1);
    request->version_ = line->copy(i1 + 1, line->count());
    request->majorVersion_ = 1;
    request->minorVersion_ = 0;

    Ref<StringList> parts = request->version_->split('/');
    if (parts->count() >= 2) {
        parts->at(0)->downcaseInsitu();
        if (parts->at(0) != "http") throw UnsupportedVersion();
        parts = parts->at(1)->split('.');
        if (parts->count() >= 2) {
            request->majorVersion_ = parts->at(0)->toNumber<int>();
            request->minorVersion_ = parts->at(1)->toNumber<int>();
        }
    }

    if (request->majorVersion_ > 1) throw UnsupportedVersion();
}

void HttpClientConnection::onHeaderReceived(HttpMessage *message)
{
    Ref<HttpRequest> request = message;
    request->host_ = request->value("Host");
    request->host_->downcaseInsitu();
    if (request->host_ == "") throw BadRequest();
}

} // namespace ccnode
