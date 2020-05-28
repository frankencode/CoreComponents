/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/net/exceptions>
#include <cc/Format>
#include <sys/types.h>
#include <sys/socket.h>

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

HostNameResolutionError::HostNameResolutionError(const String &hostName):
    hostName_{hostName}
{}

String HostNameResolutionError::hostName() const
{
    return hostName_;
}

String HostNameResolutionError::message() const
{
    return Format{"Failed to resolve host name \"%%\""} << hostName_;
}

InvalidAddressSyntax::InvalidAddressSyntax(const String &address):
    address_{address}
{}

String InvalidAddressSyntax::address() const
{
    return address_;
}

String InvalidAddressSyntax::message() const
{
    return Format{"Failed to parse network address \"%%\""} << address_;
}

}} // namespace cc::net
