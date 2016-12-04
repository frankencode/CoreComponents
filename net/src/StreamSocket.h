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

/** \class StreamSocket StreamSocket.h cc/net/StreamSocket
  * \brief Connection oriented byte-sequential communication channel
  */
class StreamSocket: public SystemStream
{
public:
    static Ref<StreamSocket> listen(const SocketAddress *address, int backlog = 1024);
    static Ref<StreamSocket> connect(const SocketAddress *address);
    static void connect(Ref<StreamSocket> *first, Ref<StreamSocket> *second);

    Ref<StreamSocket> accept();
    void shutdown(int how = SHUT_RDWR);

    // FIXME: confusing: local address, peer address or listening address?
    const SocketAddress *address() const;

    void setRecvTimeout(double interval);
    void setSendTimeout(double interval);

protected:
    StreamSocket(int fd);
    StreamSocket(const SocketAddress *address);
    void listen(int backlog = 1024);
    int accept(SocketAddress *clientAddress);
    void connect();

    Ref<SocketAddress> address_;
    bool connected_;
};

}} // namespace cc::net
