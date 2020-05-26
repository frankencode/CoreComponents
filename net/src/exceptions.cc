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

HostNameResolutionError::HostNameResolutionError(const string &hostName):
    hostName_{hostName}
{}

string HostNameResolutionError::hostName() const
{
    return hostName_;
}

string HostNameResolutionError::message() const
{
    return Format{"Failed to resolve host name \"%%\""} << hostName_;
}

InvalidAddressSyntax::InvalidAddressSyntax(const string &address):
    address_{address}
{}

string InvalidAddressSyntax::address() const
{
    return address_;
}

string InvalidAddressSyntax::message() const
{
    return Format{"Failed to parse network address \"%%\""} << address_;
}

}} // namespace cc::net
