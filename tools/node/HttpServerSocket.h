/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/http/HttpSocket>
#include "ServiceInstance.h"

namespace ccnode {

using namespace cc;
using namespace cc::net;

class ClientHelloContext;

class HttpServerSocket: public HttpSocket
{
public:
    static Ref<HttpServerSocket> accept(StreamSocket *listeningSocket);
    ServiceInstance *handshake();

    bool isSecure() const { return mode_ & Secure; }
    void upgradeToSecureTransport();

    int read(ByteArray *data) override;
    void write(const ByteArray *data) override;
    void write(const StringList *parts) override;

private:
    friend class ClientHelloContext;

    HttpServerSocket(const SocketAddress *address, int mode);
    ~HttpServerSocket();

    static int onClientHello(gnutls_session_t session);
    void initSession();

    virtual bool waitInput() override;
    virtual void ioException(Exception &ex) const override;

    double t0_;
    double te_;
};

} // namespace ccnode
