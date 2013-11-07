/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXNODE_SERVICEWORKER_H
#define FLUXNODE_SERVICEWORKER_H

#include <flux/Thread.h>
#include <flux/Channel.h>
#include <flux/StreamSocket.h>
#include <flux/Map.h>
#include "ClientConnection.h"

namespace fluxnode
{

using namespace flux;

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

	static void logDelivery(ClientConnection *client, int statusCode, size_t bytesWritten = 0);
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
