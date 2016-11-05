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

/** \class HostUnreachable DatagramSocket.h cc/net/DatagramSocket
  * \brief An earlier datagram could not be delivered
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

/** \class DatagramSocket DatagramSocket.h cc/net/DatagramSocket
  * \brief Datagram socket
  */
class DatagramSocket: public SystemStream
{
public:
    /** create a new datagram socket to communicate with one or more peers
      * \address local address to bind to the socket
      */
    static Ref<DatagramSocket> open(const SocketAddress *address);

    /** Create a pair of connected local datagram sockets
      * \param first return the first socket
      * \param second return the second socket
      */
    static void connect(Ref<DatagramSocket> *first, Ref<DatagramSocket> *second);

    /** Enabled/disable sending broadcasts on this socket
      * \param on if true enable broadcasting
      * \see SocketAddress::createBroadcast()
      */
    void allowBroadcasting(bool on);

    /** Set the incoming socket buffer size
      * \param newSize new incoming buffer size in bytes
      */
    void setRecvBufferSize(int newSize);

    /** Set the outgoing socket buffer size
      * \param newSize new outgoing buffer size in bytes
      */
    void setSendBufferSize(int newSize);

    /** Receive a datagram
      * \param peerAddress peer address from which the datagram originates from
      * \param buffer data buffer to hold the datagram received
      * \return number of bytes received
      * This method throws HostUnreachable if an ICMP error from an earlier sendTo() is received.
      */
    virtual int recvFrom(Ref<SocketAddress> *peerAddress, ByteArray *buffer);

    /** Send a datagram
      * \param peerAddress destination host address
      * \param message the message to send
      * \return number of bytes send
      * \see ByteArray::select()
      */
    virtual int sendTo(const SocketAddress *peerAddress, const ByteArray *message);

    /** Restrict communication to a single source
      * \param address unicast or multicast address to receive datagrams from
      */
    void connect(const SocketAddress *peerAddress);

protected:
    DatagramSocket(int fd = 0);
    DatagramSocket(const SocketAddress *localAddress);
    int addressFamily_;
};

}} // namespace cc::net
