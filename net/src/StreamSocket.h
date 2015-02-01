/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXNET_STREAMSOCKET_H
#define FLUXNET_STREAMSOCKET_H

#include <flux/SystemStream>
#include <flux/net/SocketAddress>

namespace flux {
namespace net {

/** \brief Connection oriented byte-sequential communication channel
  * \see ConnectionMonitor
  */
class StreamSocket: public SystemStream
{
public:
    static Ref<StreamSocket> listen(SocketAddress *address);
    static Ref<StreamSocket> connect(SocketAddress *address);

    SocketAddress *address() const;
    bool getPeerAddress(SocketAddress *address);

    Ref<StreamSocket> accept();
    void shutdown(int how = SHUT_RDWR);

    void setRecvTimeout(double interval);
    void setSendTimeout(double interval);

protected:
    StreamSocket(SocketAddress *address);
    StreamSocket(SocketAddress *address, int fdc);
    void bind();
    void listen(int backlog = 8);
    void connect();

    Ref<SocketAddress> address_;
    bool connected_;
};

}} // namespace flux::net

#endif // FLUXNET_STREAMSOCKET_H
