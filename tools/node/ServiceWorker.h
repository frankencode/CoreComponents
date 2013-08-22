/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FNODE_SERVICEWORKER_H
#define FNODE_SERVICEWORKER_H

#include <fkit/Thread.h>
#include <fkit/Channel.h>
#include <fkit/StreamSocket.h>
#include <fkit/Map.h>
#include "ClientConnection.h"

namespace fnode
{

using namespace fkit;

class ServiceWorker;

typedef Channel< Ref<ClientConnection> > PendingConnections;
typedef Channel< Ref<SocketAddress> > ClosedConnections;

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

	static void logDelivery(ClientConnection *client, int statusCode);
	virtual void run();

	Ref<ServiceInstance> serviceInstance_;
	Ref<ServiceDelegate> serviceDelegate_;

	Ref<PendingConnections> pendingConnections_;
	Ref<ClosedConnections> closedConnections_;

	Ref<ClientConnection> client_;
	Ref<Response> response_;
};

} // namespace fnode

#endif // FNODE_SERVICEWORKER_H
