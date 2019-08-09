/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <gnutls/gnutls.h>
#include <cc/Format>
#include <cc/http/exceptions>

namespace cc {
namespace http {

ProtocolException::ProtocolException(int statusCode, const String &message):
    statusCode_{statusCode},
    message_{message}
{}

int ProtocolException::statusCode() const
{
    return statusCode_;
}

String ProtocolException::message() const
{
    return message_;
}

SecurityError::SecurityError(int errorCode, const SocketAddress *peerAddress):
    errorCode_{errorCode},
    peerAddress_{peerAddress}
{}

String SecurityError::message() const
{
    const char *s = gnutls_strerror(errorCode_);
    if (peerAddress_)
        return Format() << peerAddress_ << ": " << s;
    return s;
}

}} // namespace cc::http
