/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <flux/System.h>
#include <flux/net/SocketAddress.h>
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
