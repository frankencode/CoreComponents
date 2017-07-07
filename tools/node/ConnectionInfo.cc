/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/System>
#include <cc/net/SocketAddress>
#include "ConnectionInfo.h"

namespace ccnode {

Ref<ConnectionInfo> ConnectionInfo::create(const SocketAddress *remoteAddress)
{
    return new ConnectionInfo(remoteAddress);
}

ConnectionInfo::ConnectionInfo(const SocketAddress *remoteAddress):
    remoteAddress_(remoteAddress),
    priority_(0),
    originAddress_(remoteAddress->networkPrefix()),
    arrivalTime_(System::now()),
    departureTime_(arrivalTime_)
{}

void ConnectionInfo::updateDepartureTime()
{
    departureTime_ = System::now();
}

} // namespace ccnode
