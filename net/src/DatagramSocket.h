/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/SystemStream>
#include <cc/net/SocketAddress>

namespace cc {
namespace net {

/*! \brief an earlier datagram could not be delivered
  *
  * \see DatagramSocket::recvFrom()
  */
class HostUnreachable: public Exception
{
public:
    ~HostUnreachable() throw() {}

    /*! readable error message
      */
    virtual String message() const;
};

/*! \brief datagram socket
  */
class DatagramSocket: public SystemStream
{
public:
    /*! create a new datagram socket to communicate with one or more peers
      * \address address to bind to the socket
      */
    static Ref<DatagramSocket> open(const SocketAddress *address);

    /*! create a local datagram socket connection
      */
    static void connect(Ref<DatagramSocket> *first, Ref<DatagramSocket> *second);

    /*! set the incoming socket buffer size
      */
    void setRecvBufferSize(int newSize);

    /*! set the outgoing socket buffer size
      */
    void setSendBufferSize(int newSize);

    /*! receive a datagram
      * \arg peerAddress peer address from which the datagram originates from
      * \arg buffer data buffer to hold the datagram received
      * \ret number of bytes received
      * This method throws HostUnreachable if an ICMP error from an earlier sendTo() is received.
      */
    virtual int recvFrom(Ref<SocketAddress> *peerAddress, ByteArray *buffer);

    /*! send a datagram
      * \arg peerAddress destination host address
      * \arg message the message to send
      * \ret number of bytes send
      * \see ByteArray::select()
      */
    virtual int sendTo(const SocketAddress *peerAddress, const ByteArray *message);

    /*! restrict communication to a single source
      * \arg address unicast or multicast address to receive datagrams from
      */
    void connect(const SocketAddress *peerAddress);

protected:
    DatagramSocket(int fd = 0);
    DatagramSocket(const SocketAddress *localAddress);
    int addressFamily_;
};

}} // namespace cc::net
