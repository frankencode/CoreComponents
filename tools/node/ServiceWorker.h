/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Thread>
#include <cc/Channel>
#include <cc/PriorityQueue>
#include <cc/Map>
#include <cc/net/StreamSocket>
#include "HttpClientConnection.h"

namespace ccnode {

using namespace cc;
using namespace cc::net;

class ServiceWorker;

typedef Channel< Ref<HttpClientConnection>, PriorityQueue > PendingConnections;
typedef Channel< Ref<ConnectionInfo> > ClosedConnections;

class ServiceInstance;
class ServiceDelegate;
class HttpResponseGenerator;

class ServiceWorker: public Thread
{
public:
    static Ref<ServiceWorker> create(PendingConnections *pendingConnections, ClosedConnections *closedConnections);

    inline ServiceInstance *serviceInstance() const { return serviceInstance_; }
    inline HttpClientConnection *client() const { return client_; }

    HttpResponseGenerator *response() const;
    void autoSecureForwardings();
    void closeConnection();

private:
    ServiceWorker(PendingConnections *pendingConnections, ClosedConnections *closedConnections);
    ~ServiceWorker();

    static void logDelivery(HttpClientConnection *client, int statusCode, size_t bytesWritten = 0, const String &statusMessage = "");
    virtual void run();

    Ref<ServiceInstance> serviceInstance_;
    Ref<ServiceDelegate> serviceDelegate_;

    Ref<PendingConnections> pendingConnections_;
    Ref<ClosedConnections> closedConnections_;

    Ref<HttpClientConnection> client_;
    Ref<HttpResponseGenerator> response_;
};

} // namespace ccnode
