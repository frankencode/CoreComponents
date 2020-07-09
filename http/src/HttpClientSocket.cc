/*
 * Copyright (C) 2007-2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/http/HttpClientSocket>
#include <cc/IoMonitor>
#include <cc/System>
#include <cc/assert>
#include <gnutls/gnutls.h>

namespace cc {
namespace http {

HttpClientSocket::Instance::Instance(const SocketAddress &serverAddress, const String &serverName, const HttpClientSecurity *security):
    HttpSocket::Instance{serverAddress, (serverAddress->port() % 80 == 0 || !security) ? 0 : Secure},
    serverName_{serverName},
    security_{security},
    readyRead_{0}
{
    if ((mode_ & Secure) && !security_)
        security_ = HttpClientSecurity::createDefault();

    controlMaster_->connect(controlSlave_);
}

bool HttpClientSocket::Instance::isSecure() const
{
    return mode_ & Secure;
}

void HttpClientSocket::Instance::connect()
{
    StreamSocket::Instance::connect(address());
    initSession();
    StreamSocket::Instance::waitForReady();
    handshake();
}

void HttpClientSocket::Instance::shutdown()
{
    controlMaster_->close();
}

void HttpClientSocket::Instance::initSession()
{
    if (!(mode_ & Secure)) return;

    gnuTlsCheckSuccess(gnutls_init(&session_, GNUTLS_CLIENT));
    security_->establish(session_);
    gnuTlsCheckSuccess(gnutls_server_name_set(session_, GNUTLS_NAME_DNS, serverName_->chars(), serverName_->count()));
    initTransport();
}

void HttpClientSocket::Instance::handshake()
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

bool HttpClientSocket::Instance::waitInput()
{
    return waitFor(IoReady::Read);
}

void HttpClientSocket::Instance::ioException(Exception &ex) const
{
    // FIXME: need a thread local singleton to propagate this exception
}

}} // namespace cc::http
