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

class DeliveryWorker;

typedef PriorityChannel< Ref<HttpServerConnection> > PendingConnections;
typedef Channel< Ref<ConnectionInfo> > ClosedConnections;

class DeliveryInstance;
class DeliveryDelegate;
class HttpResponseGenerator;

class DeliveryWorker: public Thread
{
public:
    static Ref<DeliveryWorker> create(const NodeConfig *nodeConfig, PendingConnections *pendingConnections, ClosedConnections *closedConnections);

    const NodeConfig *nodeConfig() const { return nodeConfig_; }

    const DeliveryInstance *deliveryInstance() const { return deliveryInstance_; }
    HttpServerConnection *client() const { return client_; }

    HttpResponseGenerator *response() const;
    void autoSecureForwardings();
    void closeConnection();

private:
    DeliveryWorker(const NodeConfig *nodeConfig, PendingConnections *pendingConnections, ClosedConnections *closedConnections);
    ~DeliveryWorker();

    static void logDelivery(HttpServerConnection *client, int statusCode, size_t bytesWritten = 0, const String &statusMessage = "");
    void run() final;

    Ref<const DeliveryInstance> deliveryInstance_;
    Ref<DeliveryDelegate> deliveryDelegate_;

    const NodeConfig *nodeConfig_;
    Ref<PendingConnections> pendingConnections_;
    Ref<ClosedConnections> closedConnections_;

    Ref<HttpServerConnection> client_;
    Ref<HttpResponseGenerator> response_;
};

} // namespace ccnode
