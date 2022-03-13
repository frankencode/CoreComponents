/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/TlsError>
#include <cc/Format>
#include <gnutls/gnutls.h>

namespace cc {

TlsError::TlsError(int errorCode, const SocketAddress &peerAddress):
    errorCode_{errorCode},
    peerAddress_{peerAddress}
{}

String TlsError::message() const
{
    const char *s = gnutls_strerror_name(errorCode_);
    if (peerAddress_) return Format{} << peerAddress_ << ": " << s;
    return s;
}

} // namespace cc
