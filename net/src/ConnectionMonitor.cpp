/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <flux/exceptions>
#include <flux/IoMonitor>
#include <flux/net/ConnectionMonitor>

namespace flux {
namespace net {

Ref<ConnectionMonitor> ConnectionMonitor::create(StreamSocket *socket)
{
    return new ConnectionMonitor(socket);
}

ConnectionMonitor::ConnectionMonitor(StreamSocket *socket):
    socket_(socket),
    ioMonitor_(IoMonitor::create(1))
{
    ioMonitor_->addEvent(socket_, IoEvent::ReadyRead|IoEvent::ReadyWrite);
}

/** wait for connection request
  */
bool ConnectionMonitor::waitReadyAccept(double timeout) const
{
    return ioMonitor_->wait(timeout);
}

/** wait for the connection becoming established
  */
bool ConnectionMonitor::waitEstablished(double timeout) const
{
    if (ioMonitor_->wait(timeout)) {
        int error = 0;
        socklen_t len = sizeof(error);
        if (::getsockopt(socket_->fd(), SOL_SOCKET, SO_ERROR, &error, &len) == -1)
            FLUX_SYSTEM_DEBUG_ERROR(errno);

        if (error != 0) {
            errno = error;
            FLUX_SYSTEM_DEBUG_ERROR(errno);
        }
        return true;
    }

    return false;
}

}} // namespace flux::net
