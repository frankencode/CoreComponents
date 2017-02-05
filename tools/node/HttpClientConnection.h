/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include "HttpConnection.h"
#include "ConnectionInfo.h"
#include "HttpRequest.h"
#include "ServiceInstance.h"
#include "HttpClientSocket.h"

namespace ccnode {

class HttpClientConnection: public HttpConnection
{
public:
    static Ref<HttpClientConnection> open(HttpClientSocket *socket);

    inline const SocketAddress *address() const { return socket_->address(); }

    ServiceInstance *handshake();
    bool isSecure() const;

    Ref<HttpRequest> readRequest();
    void putBack(HttpRequest *request);

    inline HttpRequest *request() const { return request_; }
    inline ConnectionInfo *connectionInfo() const { return connectionInfo_; }
    inline int priority() const { return connectionInfo_->priority(); }

private:
    HttpClientConnection(HttpClientSocket *socket);

    Ref<HttpRequest> scanRequest();
    virtual void readFirstLine(LineSource *source, HttpMessage *message);
    virtual void onHeaderReceived(HttpMessage *message);

    Ref<HttpClientSocket> socket_;
    Ref<HttpRequest> request_, pendingRequest_;
    Ref<ConnectionInfo> connectionInfo_;
};

} // namespace ccnode
