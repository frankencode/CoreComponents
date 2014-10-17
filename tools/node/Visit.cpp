/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/System>
#include <flux/net/SocketAddress>
#include "Visit.h"

namespace fluxnode
{

Ref<Visit> Visit::create(SocketAddress *remoteAddress)
{
    return new Visit(remoteAddress);
}

Visit::Visit(SocketAddress *remoteAddress)
    : remoteAddress_(remoteAddress),
      priority_(0),
      originAddress_(remoteAddress->networkPrefix()),
      arrivalTime_(System::now()),
      departureTime_(arrivalTime_)
{}

void Visit::updateDepartureTime()
{
    departureTime_ = System::now();
}

} // namespace fluxnode
