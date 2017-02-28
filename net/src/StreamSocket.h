/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/SystemStream>
#include <cc/exceptions>
#include <cc/net/SocketAddress>

namespace cc {
namespace net {

/** \class StreamSocket StreamSocket.h cc/net/StreamSocket
  * \brief Connection oriented byte-sequential communication channel
  */
class StreamSocket: public SystemStream
{
public:
    /** Open a new listening socket
      * \param address local address to bind to
      * \param backlog number of connections to queue when the process is busy
      * \return new object instance
      */
    static Ref<StreamSocket> listen(const SocketAddress *localAddress, int backlog = 1024);

    /** Connect to a remote host
      * \param peerAddress address of remote host
      * \return new object instance
      */
    static Ref<StreamSocket> connect(const SocketAddress *peerAddress);

    /** Create a pair of connected local sockets
      * \param first return the first socket
      * \param second return the second socket
      */
    static void connect(Ref<StreamSocket> *first, Ref<StreamSocket> *second);

    /** Accept a connection request
      * \return new object instance
      */
    Ref<StreamSocket> accept();

    /** Returns the local or remote address associated with this socket
      */
    const SocketAddress *address() const;

    /** %Set the timeout of accumulating incoming data
      * \param interval timeout in seconds
      */
    void setRecvTimeout(double interval);

    /** %Set the timeout of accumulating outgoing data
      * \param internval timeout in seconds
      */
    void setSendTimeout(double interval);

protected:
    StreamSocket(int fd);
    StreamSocket(const SocketAddress *address);
    void listen(int backlog = 1024);
    int accept(SocketAddress *clientAddress);
    inline static int accept(StreamSocket *listeningSocket, SocketAddress *clientAddress) {
        return listeningSocket->accept(clientAddress);
    }
    void connect();

    Ref<SocketAddress> address_;
    bool connected_;
};

}} // namespace cc::net
