/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/System>
#include <cc/LineSource>
#include <cc/http/exceptions>
#include "ErrorLog.h"
#include "NodeConfig.h"
#include "HttpServerConnection.h"

namespace ccnode {

using namespace cc;
using namespace cc::http;

Ref<HttpServerConnection> HttpServerConnection::open(HttpServerSocket *socket)
{
    return new HttpServerConnection{socket};
}

HttpServerConnection::HttpServerConnection(HttpServerSocket *socket):
    HttpConnection{socket},
    socket_{socket},
    connectionInfo_{ConnectionInfo::create(socket->address())}
{
    if (ErrorLog::instance()->level() >= LogLevel::Debug)
        setupTransferLog(ErrorLog::instance()->debugStream(), socket->address()->toString());
}

const ServiceInstance *HttpServerConnection::handshake()
{
    const ServiceInstance *serviceInstance = nullptr;

    if (socket_->isSecure()) {
        serviceInstance = socket_->handshake();
    }
    else {
        String host, uri;
        Ref<HttpRequest> request = readRequest();
        host = request->host();
        uri = request->uri();
        putBack(request);

        serviceInstance = socket_->nodeConfig()->selectService(host, uri);
    }

    return serviceInstance;
}

bool HttpServerConnection::isSecure() const
{
    return socket_->isSecure();
}

Ref<HttpRequest> HttpServerConnection::readRequest()
{
    if (pendingRequest_) {
        Ref<HttpRequest> h = pendingRequest_;
        pendingRequest_ = nullptr;
        return h;
    }
    request_ = HttpRequest::create();
    request_->time_ = System::now();
    readMessage(request_);
    return request_;
}

void HttpServerConnection::putBack(HttpRequest *request)
{
    pendingRequest_ = request;
}

void HttpServerConnection::readFirstLine(LineSource *source, HttpMessage *message)
{
    String line;
    if (!source->read(&line)) throw CloseRequest{};

    HttpRequest *request = Object::cast<HttpRequest *>(message);
    request->line_ = line;

    if (line->count(' ') != 2) throw BadRequest{};

    int i0 = 0, i1 = line->find(' ');
    request->method_ = line->copy(i0, i1);
    i0 = i1 + 1; i1 = line->find(' ', i0);
    request->uri_ = line->copy(i0, i1);
    request->version_ = line->copy(i1 + 1, line->count());
    request->majorVersion_ = 1;
    request->minorVersion_ = 0;

    Ref<StringList> parts = request->version_->split('/');
    if (parts->count() >= 2) {
        mutate(parts->at(0))->upcaseInsitu();
        if (parts->at(0) != "HTTP") throw UnsupportedVersion{};
        parts = parts->at(1)->split('.');
        if (parts->count() >= 2) {
            request->majorVersion_ = parts->at(0)->toNumber<int>();
            request->minorVersion_ = parts->at(1)->toNumber<int>();
        }
    }

    if (request->majorVersion_ > 1) throw UnsupportedVersion{};
}

void HttpServerConnection::onHeaderReceived(HttpMessage *message)
{
    Ref<HttpRequest> request = message;
    request->host_ = request->value("Host");
    mutate(request->host_)->downcaseInsitu();
    if (request->host_ == "") throw BadRequest{};
}

} // namespace ccnode
