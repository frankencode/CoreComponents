/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/System.h>
#include "ErrorLog.h"
#include "ClientConnection.h"
#include "ConnectionManager.h"

namespace fluxnode
{

Ref<ConnectionManager> ConnectionManager::create(int serviceWindow)
{
	return new ConnectionManager(serviceWindow);
}

ConnectionManager::ConnectionManager(int serviceWindow)
	: closedConnections_(ClosedConnections::create()),
	  connectionCounts_(ConnectionCounts::create()),
	  visits_(Visits::create()),
	  serviceWindow_(serviceWindow)
{
	FLUXNODE_NOTICE() << "Service window of " << serviceWindow << "s will be used to prioritize connections" << nl;
}

void ConnectionManager::cycle()
{
	for (int i = 0, n = closedConnections_->count(); i < n; ++i) {
		Ref<Visit> visit = closedConnections_->pop();
		visits_->append(visit);
	}

	if (visits_->count() > 0)
	{
		double t1 = System::now() - serviceWindow_;

		while (visits_->count() > 0)
		{
			if (visits_->at(0)->departureTime() >= t1) break;

			Ref<Visit> visit = visits_->pop(0);
			uint64_t origin = visit->remoteAddress()->networkPrefix();
			int count = 0;
			int index = 0;
			if (!connectionCounts_->lookup(origin, &count, &index)) continue;

			if (count == 1) connectionCounts_->removeAt(index);
			else connectionCounts_->valueAt(index) = count - 1;
		}
	}
}

void ConnectionManager::prioritize(ClientConnection *client)
{
	uint64_t origin = client->address()->networkPrefix();
	int count = 0;
	int index = 0;
	if (!connectionCounts_->insert(origin, 1, &count, &index))
		connectionCounts_->valueAt(index) = count + 1;
	client->visit()->setPriority(count < 8 ? 0 : -count);
}

} // namespace fluxnode
