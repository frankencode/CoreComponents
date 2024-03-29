/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/HttpConnectionManager>
#include <cc/httpDebug>
#include <cc/System>

namespace cc {

struct HttpConnectionManager::State: public Object::State
{
    State(const HttpServerConfig &nodeConfig):
        nodeConfig_{nodeConfig},
        serviceWindow_{nodeConfig.serviceWindow()},
        connectionLimit_{nodeConfig.connectionLimit()}
    {
    }

    const HttpLoggingServiceInstance &errorLoggingInstance() const
    {
        return nodeConfig_.errorLoggingInstance();
    }

    void cycle()
    {
        HttpClientConnection visit;
        while (closedConnections_.count() > 0) {
            closedConnections_.popFront(&visit);
            visits_.append(visit);
        }

        if (visits_.count() > 0)
        {
            double t1 = System::now() - serviceWindow_;

            while (visits_.count() > 0)
            {
                if (visits_.at(0).departureTime() >= t1) break;

                HttpClientConnection visit = visits_.first();
                visits_.popFront();
                uint64_t origin = visit.peerAddress().networkPrefix();
                Locator target;
                if (!connectionCounts_.find(origin, &target)) continue;

                if (connectionCounts_.at(target).value() == 1) connectionCounts_.removeAt(target);
                else --connectionCounts_.at(target).value();
            }
        }
    }

    bool accept(InOut<HttpClientConnection> client)
    {
        uint64_t origin = client->peerAddress().networkPrefix();
        Locator target;
        if (!connectionCounts_.insert(origin, 1, &target)) {
            if (connectionLimit_ > 0) {
                if (connectionCounts_.at(target).value() >= connectionLimit_)
                    return false;
            }
            ++connectionCounts_.at(target).value();
        }
        client->setPriority(connectionCounts_.at(target).value() < 8 ? 0 : -connectionCounts_.at(target).value());
        return true;
    }

    Channel<HttpClientConnection> closedConnections_;

    HttpServerConfig nodeConfig_;
    Map<uint64_t, int> connectionCounts_;
    List<HttpClientConnection> visits_;
    long serviceWindow_;
    long connectionLimit_;
};

HttpConnectionManager::HttpConnectionManager(const HttpServerConfig &config):
    Object{new State{config}}
{}

Channel<HttpClientConnection> &HttpConnectionManager::closedConnections()
{
    return me().closedConnections_;
}

void HttpConnectionManager::cycle()
{
    return me().cycle();
}

bool HttpConnectionManager::accept(InOut<HttpClientConnection> client)
{
    return me().accept(client);
}

HttpConnectionManager::State &HttpConnectionManager::me()
{
    return Object::me.as<State>();
}

} // namespace cc
