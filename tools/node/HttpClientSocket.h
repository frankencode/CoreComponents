/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <gnutls/gnutls.h>
#include <cc/net/StreamSocket>
#include "ServiceInstance.h"

namespace ccnode {

using namespace cc;
using namespace cc::net;

class ClientHelloContext;

class HttpClientSocket: public StreamSocket
{
public:
    static Ref<HttpClientSocket> accept(StreamSocket *listeningSocket);
    ServiceInstance *handshake();

    bool isSecure() const { return mode_ & Secure; }
    void upgradeToSecureTransport();

    int read(ByteArray *data) override;
    void write(const ByteArray *data) override;
    void write(const StringList *parts) override;

private:
    friend class ClientHelloContext;

    enum Mode {
        Plaintext = 1,
        Secure    = 2,
        Connected = 4,
        Open      = 8
    };

    HttpClientSocket(const SocketAddress *address, int mode);
    ~HttpClientSocket();

    static int onClientHello(gnutls_session_t session);
    void sessionInit();
    void waitInput();

    bool gnuTlsCheckSuccess(int ret);
    void gnuTlsCheckError(int ret);
    static bool gnuTlsCheckSuccess(int ret, const SocketAddress *peerAddress);
    static void gnuTlsCheckError(int ret, const SocketAddress *peerAddress);
    static ssize_t gnuTlsPull(gnutls_transport_ptr_t ctx, void *data, size_t size);
    static ssize_t gnuTlsPushVec(gnutls_transport_ptr_t ctx, const giovec_t *iov, int iovcnt);

    int mode_;
    gnutls_session_t session_;
    double t0_;
    double te_;
};

} // namespace ccnode
