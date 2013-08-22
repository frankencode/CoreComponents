/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <fkit/ThreadFactory.h>
#include "ErrorLog.h"
#include "NodeConfig.h"
#include "ServiceInstance.h"
#include "WorkerPool.h"

namespace fnode
{

Ref<WorkerPool> WorkerPool::create(ServiceInstance *serviceInstance)
{
	return new WorkerPool(serviceInstance);
}

WorkerPool::WorkerPool(ServiceInstance *serviceInstance)
	: serviceInstance_(serviceInstance),
	  closedConnections_(ClosedConnections::create()),
	  serviceWorkers_(ServiceWorkers::create(serviceInstance->concurrency()))
{
	debug() << "Creating worker pool for " << serviceInstance->serviceName() << " service..." << nl;
	debug() << "concurrency = " << serviceInstance->concurrency() << ", "
	        << "stack_size = " << serviceInstance->stackSize() << nl;
	debug() << "host = \"" << serviceInstance->host() << "\", "
	        << "uri = \"" << serviceInstance->uri() << "\"" << nl;

	Ref<ThreadFactory> threadFactory = ThreadFactory::create();
	/*threadFactory->setStackSize(serviceInstance->stackSize());*/

	for (int i = 0; i < serviceWorkers_->size(); ++i) {
		Ref<ServiceWorker> worker = ServiceWorker::create(serviceInstance_, closedConnections_);
		serviceWorkers_->at(i) = worker;
		threadFactory->start(worker);
	}
}

WorkerPool::~WorkerPool()
{
	debug() << "Terminating worker pool for service " << serviceInstance_->serviceName() << "..." << nl;
}

void WorkerPool::dispatch(ClientConnection *client)
{
	for (int i = 0; i < serviceWorkers_->size(); ++i) {
		ServiceWorker *worker = serviceWorkers_->at(i);
		if (worker->pendingConnections()->size() == 0) {
			worker->pendingConnections()->push(client);
			return;
		}
	}

	ServiceWorker *workerCandidate = serviceWorkers_->at(0);
	int minPending = workerCandidate->pendingConnections()->size();
	for (int i = 1; i < serviceWorkers_->size(); ++i) {
		ServiceWorker *worker = serviceWorkers_->at(i);
		if (worker->pendingConnections()->size() < minPending) {
			workerCandidate = worker;
			minPending = worker->pendingConnections()->size();
		}
	}

	debug() << "Assigning client connection to " << workerCandidate->serviceInstance()->serviceName() << " service worker " << (void*)workerCandidate << nl;
	workerCandidate->pendingConnections()->push(client);
}

} // namespace fnode
