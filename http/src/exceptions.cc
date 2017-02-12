/*
 * Copyright (C) 2007-2016 Frank Mertens.
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

TlsError::TlsError(int errorCode, const SocketAddress *peerAddress):
    errorCode_(errorCode),
    peerAddress_(peerAddress)
{}

String TlsError::message() const
{
    return Format() << peerAddress_ << ": " << gnutls_strerror(errorCode_);
}

}} // namespace cc::http
