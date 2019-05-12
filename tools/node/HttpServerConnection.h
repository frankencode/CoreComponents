/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/http/HttpConnection>
#include "ConnectionInfo.h"
#include "HttpRequest.h"
#include "ServiceInstance.h"
#include "HttpServerSocket.h"

namespace ccnode {

using namespace cc::http;

/** \brief server-side logical HTTP connection end
  */
class HttpServerConnection: public HttpConnection
{
public:
    static Ref<HttpServerConnection> open(HttpServerSocket *socket);

    const SocketAddress *address() const { return socket_->address(); }

    ServiceInstance *handshake();
    bool isSecure() const;

    Ref<HttpRequest> readRequest();
    void putBack(HttpRequest *request);

    HttpRequest *request() const { return request_; }
    ConnectionInfo *connectionInfo() const { return connectionInfo_; }
    int priority() const { return connectionInfo_->priority(); }

private:
    HttpServerConnection(HttpServerSocket *socket);

    Ref<HttpRequest> scanRequest();
    void readFirstLine(LineSource *source, HttpMessage *message) override;
    void onHeaderReceived(HttpMessage *message) override;

    Ref<HttpServerSocket> socket_;
    Ref<HttpRequest> request_, pendingRequest_;
    Ref<ConnectionInfo> connectionInfo_;
};

} // namespace ccnode
