/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/types>
#include <cc/List>
#include <cc/Map>
#include "ServiceWorker.h"
#include "ConnectionInfo.h"

namespace ccnode {

using namespace cc;

class HttpClientConnection;

class ConnectionManager: public Object
{
public:
    static Ref<ConnectionManager> create();

    inline ClosedConnections *closedConnections() const { return closedConnections_; }

    void cycle();
    bool accept(HttpClientConnection *client);

private:
    ConnectionManager();

    typedef Map<uint64_t, int> ConnectionCounts;
    typedef List< Ref<ConnectionInfo> > Visits;

    Ref<ClosedConnections> closedConnections_;
    Ref<ConnectionCounts> connectionCounts_;
    Ref<Visits> visits_;
    int serviceWindow_;
    int connectionLimit_;
};

} // namespace ccnode
