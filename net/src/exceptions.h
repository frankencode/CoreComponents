/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/exceptions>

namespace cc {
namespace net {

class NetworkError: public SystemError
{
public:
    NetworkError(int errorCode): SystemError(errorCode) {}
    ~NetworkError() throw() {}
};

/** \class HostUnreachable exceptions.h cc/net/exceptions
  * \brief An earlier datagram could not be delivered
  *
  * \see DatagramSocket::recvFrom()
  */
class HostUnreachable: public NetworkError
{
public:
    HostUnreachable();
    ~HostUnreachable() throw() {}
};

/** \class ConnectionRefused exceptions.h cc/net/exceptions
  * \brief No-one is listening on the remote address
  */
class ConnectionRefused: public NetworkError
{
public:
    ConnectionRefused();
    ~ConnectionRefused() throw() {}
};

/** \class NetworkUnreachable exceptions.h cc/net/exceptions
  * \brief Network is unreachable
  */
class NetworkUnreachable: public NetworkError
{
public:
    NetworkUnreachable();
    ~NetworkUnreachable() throw() {}
};

/** \class ConnectionTimeoute exceptions.h cc/net/exceptions
  * \brief Timeout while attempting connection
  * The networking layer reached its maximum timeout for attempting connection to a peer.
  * The remote server may overloaded and unable to accept the connection.
  */
class ConnectionTimeout: public NetworkError
{
public:
    ConnectionTimeout();
    ~ConnectionTimeout() throw() {}
};

}} // namespace cc::net
