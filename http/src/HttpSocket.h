/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <gnutls/gnutls.h>
#include <cc/net/StreamSocket>
#include <cc/http/exceptions>

namespace cc {
namespace http {

using namespace cc::net;

/** \brief Common HTTP socket with seemless TLS encryption
  */
class HttpSocket: public StreamSocket
{
protected:
    enum Mode {
        Secure    = 1,
        Connected = 2,
        Open      = 4
    };

    HttpSocket(const SocketAddress *address, int mode);

    virtual bool waitInput() = 0;
    virtual void ioException(Exception &ex) const = 0;

    void initTransport();

    bool gnuTlsCheckSuccess(int ret);
    void gnuTlsCheckError(int ret);
    static bool gnuTlsCheckSuccess(int ret, const SocketAddress *peerAddress);
    static void gnuTlsCheckError(int ret, const SocketAddress *peerAddress);

    static ssize_t gnuTlsPull(gnutls_transport_ptr_t ctx, void *data, size_t size);
    static ssize_t gnuTlsPushVec(gnutls_transport_ptr_t ctx, const giovec_t *iov, int iovcnt);

    int mode_;
    gnutls_session_t session_;
};

}} // namespace cc::http
