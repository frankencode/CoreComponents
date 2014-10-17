/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXNODE_CONNECTIONMANAGER_H
#define FLUXNODE_CONNECTIONMANAGER_H

#include <flux/types>
#include <flux/List>
#include <flux/Map>
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
