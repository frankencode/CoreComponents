/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXNODE_CONNECTIONMANAGER_H
#define FLUXNODE_CONNECTIONMANAGER_H

#include <flux/types.h>
#include <flux/List.h>
#include <flux/Map.h>
#include "ServiceWorker.h"
#include "Visit.h"

namespace fluxnode
{

using namespace flux;

class ClientConnection;

class ConnectionManager: public Object
{
public:
	static Ref<ConnectionManager> create(int serviceWindow = 30);

	inline ClosedConnections *closedConnections() const { return closedConnections_; }

	void cycle();
	void prioritize(ClientConnection *client);

private:
	ConnectionManager(int serviceWindow);

	typedef Map<uint64_t, int> ConnectionCounts;
	typedef List< Ref<Visit> > Visits;

	Ref<ClosedConnections> closedConnections_;
	Ref<ConnectionCounts> connectionCounts_;
	Ref<Visits> visits_;
	int serviceWindow_;
};

} // namespace fluxnode

#endif // FLUXNODE_CONNECTIONMANAGER_H
