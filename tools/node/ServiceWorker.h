/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXNODE_SERVICEWORKER_H
#define FLUXNODE_SERVICEWORKER_H

#include <flux/Thread>
#include <flux/Channel>
#include <flux/PriorityQueue>
#include <flux/Map>
#include <flux/net/StreamSocket>
#include "ClientConnection.h"

namespace fluxnode
{

using namespace flux;
using namespace flux::net;

class ServiceWorker;

typedef Channel< Ref<ClientConnection>, PriorityQueue > PendingConnections;
typedef Channel< Ref<Visit> > ClosedConnections;

class ServiceInstance;
class ServiceDelegate;
class Response;

class ServiceWorker: public Thread
{
public:
    static Ref<ServiceWorker> create(ServiceInstance *serviceInstance, ClosedConnections *closedConnections);

    inline ServiceInstance *serviceInstance() const { return serviceInstance_; }

    inline PendingConnections *pendingConnections() const { return pendingConnections_; }
    inline ClientConnection *client() const { return client_; }

    Response *response() const;
    void close();

private:
    ServiceWorker(ServiceInstance *serviceInstance, ClosedConnections *closedConnections);
    ~ServiceWorker();

    static void logDelivery(ClientConnection *client, int statusCode, size_t bytesWritten = 0);
    static void logVisit(Visit *visit);
    virtual void run();

    Ref<ServiceInstance> serviceInstance_;
    Ref<ServiceDelegate> serviceDelegate_;

    Ref<PendingConnections> pendingConnections_;
    Ref<ClosedConnections> closedConnections_;

    Ref<ClientConnection> client_;
    Ref<Response> response_;
};

} // namespace fluxnode

#endif // FLUXNODE_SERVICEWORKER_H
