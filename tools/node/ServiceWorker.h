/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Thread>
#include <cc/PriorityChannel>
#include <cc/Channel>
#include "HttpServerConnection.h"

namespace ccnode {

using namespace cc;
using namespace cc::net;

class ServiceWorker;

typedef PriorityChannel< Ref<HttpServerConnection> > PendingConnections;
typedef Channel< Ref<ConnectionInfo> > ClosedConnections;

class ServiceInstance;
class ServiceDelegate;
class HttpResponseGenerator;

class ServiceWorker: public Thread
{
public:
    static Ref<ServiceWorker> create(const NodeConfig *nodeConfig, PendingConnections *pendingConnections, ClosedConnections *closedConnections);

    const NodeConfig *nodeConfig() const { return nodeConfig_; }

    ServiceInstance *serviceInstance() const { return serviceInstance_; }
    HttpServerConnection *client() const { return client_; }

    HttpResponseGenerator *response() const;
    void autoSecureForwardings();
    void closeConnection();

private:
    ServiceWorker(const NodeConfig *nodeConfig, PendingConnections *pendingConnections, ClosedConnections *closedConnections);
    ~ServiceWorker();

    static void logDelivery(HttpServerConnection *client, int statusCode, size_t bytesWritten = 0, const String &statusMessage = "");
    virtual void run();

    Ref<ServiceInstance> serviceInstance_;
    Ref<ServiceDelegate> serviceDelegate_;

    const NodeConfig *nodeConfig_;
    Ref<PendingConnections> pendingConnections_;
    Ref<ClosedConnections> closedConnections_;

    Ref<HttpServerConnection> client_;
    Ref<HttpResponseGenerator> response_;
};

} // namespace ccnode
