/*
 * Copyright (C) 2007-2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/http/ConnectionManager>
#include <cc/http/NodeConfig>
#include <cc/http/HttpServerConnection>
#include <cc/http/debug>
#include <cc/System>

namespace cc {
namespace http {

Ref<ConnectionManager> ConnectionManager::create(const NodeConfig *nodeConfig)
{
    return new ConnectionManager{nodeConfig};
}

ConnectionManager::ConnectionManager(const NodeConfig *nodeConfig):
    nodeConfig_{nodeConfig},
    closedConnections_{ClosedConnections::create()},
    connectionCounts_{ConnectionCounts::create()},
    visits_{Visits::create()},
    serviceWindow_{nodeConfig->serviceWindow()},
    connectionLimit_{nodeConfig->connectionLimit()}
{
    CCNODE_NOTICE()
        << "Service window of " << serviceWindow_ << "s will be used to prioritize connections" << nl;

    if (connectionLimit_ > 0) {
        CCNODE_NOTICE()
            << "No more than " << connectionLimit_ << " simultaneous connections are allowed for each peer" << nl;
    }
}

const LoggingInstance *ConnectionManager::errorLoggingInstance() const
{
    return nodeConfig_->errorLoggingInstance();
}

void ConnectionManager::cycle()
{
    for (int i = 0, n = closedConnections_->count(); i < n; ++i) {
        Ref<ConnectionInfo> visit = closedConnections_->pop();
        visits_->append(visit);
    }

    if (visits_->count() > 0)
    {
        double t1 = System::now() - serviceWindow_;

        while (visits_->count() > 0)
        {
            if (visits_->at(0)->departureTime() >= t1) break;

            Ref<ConnectionInfo> visit = visits_->front();
            visits_->popFront();
            uint64_t origin = visit->remoteAddress()->networkPrefix();
            ConnectionCounts::Iterator target;
            if (!connectionCounts_->lookup(origin, &target)) continue;

            if (target->value() == 1) connectionCounts_->remove(target);
            else target->setValue(target->value() - 1);
        }
    }
}

bool ConnectionManager::accept(HttpServerConnection *client)
{
    uint64_t origin = client->address()->networkPrefix();
    int count = 0;
    ConnectionCounts::Index index = 0;
    if (!connectionCounts_->insert(origin, 1, &count, &index)) {
        if (connectionLimit_ > 0) {
            if (count >= connectionLimit_)
                return false;
        }
        connectionCounts_->setValueAt(index, count + 1);
    }
    client->connectionInfo()->setPriority(count < 8 ? 0 : -count);
    return true;
}

}} // namespace cc::http
