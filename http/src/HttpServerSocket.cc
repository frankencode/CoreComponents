/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <gnutls/gnutls.h>
#include <cc/assert>
#include <cc/System>
#include <cc/http/HttpServerSocket>

namespace cc {
namespace http {

Ref<HttpServerSocket> HttpServerSocket::connect(const SocketAddress *serverAddress, String serverName, SecuritySettings *security, double timeout)
{
    Ref<HttpServerSocket> socket = new HttpServerSocket(serverAddress, serverName, security, timeout);
    socket->connect();
    socket->initSession();
    socket->handshake();

    return socket;
}

HttpServerSocket::HttpServerSocket(const SocketAddress *serverAddress, String serverName, SecuritySettings *security, double timeout):
    HttpSocket(serverAddress, (serverAddress->port() % 80 == 0) ? 0 : Secure),
    serverName_(serverName),
    security_(security),
    t0_(System::now()),
    te_(t0_ + timeout)
{
    if ((mode_ & Secure) && !security_)
        security_ = SecuritySettings::createDefault();
}

void HttpServerSocket::connect()
{
    connect(address());
    int timeout_ms = (te_ < t0_) ? -1 : int((te_ - t0_) * 1000);
    if (!poll(IoReadyWrite, timeout_ms)) throw RequestTimeout();
    mode_ |= Connected;
}

void HttpServerSocket::initSession()
{
    if (!(mode_ & Secure)) return;

    gnuTlsCheckSuccess(gnutls_init(&session_, GNUTLS_CLIENT));
    security_->establish(session_);
    gnuTlsCheckSuccess(gnutls_server_name_set(session_, GNUTLS_NAME_DNS, serverName_->chars(), serverName_->count()));
    initTransport();
}

void HttpServerSocket::handshake()
{
    if (!(mode_ & Secure)) return;

    while (true) {
        int ret = gnutls_handshake(session_);
        if (ret == GNUTLS_E_SUCCESS) break;
        if (ret < 0) {
            if (gnutls_error_is_fatal(ret))
                gnuTlsCheckSuccess(ret);
        }
    }

    mode_ |= Open;
}

bool HttpServerSocket::waitInput()
{
    double d = te_ - System::now();
    if (d <= 0) throw RequestTimeout();
    return poll(IoReadyRead, d * 1000);
}

void HttpServerSocket::ioException(Exception &ex) const
{
    // TODO: need a HttpConnectionHandler
}

}} // namespace cc::http
