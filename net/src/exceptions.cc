/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <cc/net/exceptions>

namespace cc {
namespace net {

NetworkError::NetworkError(int errorCode):
    SystemError{errorCode}
{}

HostUnreachable::HostUnreachable():
    NetworkError{EHOSTUNREACH}
{}

ConnectionRefused::ConnectionRefused():
    NetworkError{ECONNREFUSED}
{}

NetworkUnreachable::NetworkUnreachable():
    NetworkError{ENETUNREACH}
{}

ConnectionTimeout::ConnectionTimeout():
    NetworkError{ETIMEDOUT}
{}

HostNameResolutionError::HostNameResolutionError(const String &message):
    UsageError{message}
{}

}} // namespace cc::net
