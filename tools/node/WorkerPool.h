/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXNODE_WORKERPOOL_H
#define FLUXNODE_WORKERPOOL_H

#include "ServiceWorker.h"

namespace fluxnode
{

class WorkerPool: public Object
{
public:
	static Ref<WorkerPool> create(ServiceInstance *serviceInstance);

	inline ServiceInstance *serviceInstance() const { return serviceInstance_; }

	void dispatch(ClientConnection *client);

private:
	WorkerPool(ServiceInstance *serviceInstance);
	~WorkerPool();

	Ref<ServiceInstance> serviceInstance_;
	Ref<ClosedConnections> closedConnections_;

	typedef Array< Ref<ServiceWorker> > ServiceWorkers;
	Ref<ServiceWorkers> serviceWorkers_;
};

typedef Array< Ref<WorkerPool> > WorkerPools;

} // namespace fluxnode

#endif // FLUXNODE_WORKERPOOL_H
