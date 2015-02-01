/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXNET_CONNECTIONMONITOR_H
#define FLUXNET_CONNECTIONMONITOR_H

#include <flux/net/StreamSocket>

namespace flux { class IoMonitor; }

namespace flux {
namespace net {

/** \brief monitor connection state of a stream socket
  * \see StreamSocket
  */
class ConnectionMonitor: public Object
{
public:
    static Ref<ConnectionMonitor> create(StreamSocket *socket);

    bool waitReadyAccept(double timeout) const;
    bool waitEstablished(double timeout) const;

private:
    ConnectionMonitor(StreamSocket *socket);
    StreamSocket *socket_;
    IoMonitor *ioMonitor_;
};

}} // namespace flux::net

#endif // FLUXNET_CONNECTIONMONITOR_H
