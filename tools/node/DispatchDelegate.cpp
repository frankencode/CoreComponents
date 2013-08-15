/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "NodeLog.h"
#include "DispatchInstance.h"
#include "DispatchDelegate.h"

namespace fnode
{

Ref<DispatchDelegate> DispatchDelegate::create(ServiceWorker *worker) {
	return new DispatchDelegate(worker);
}

void DispatchDelegate::process(Request *request)
{
	debug() << "Selecting service for request to host = \"" << request->host() << "\", target = \"" << request->target() << "\"" << nl;
	WorkerPools *workerPools = dispatchInstance_->workerPools();
	for (int i = 0; i < workerPools->size(); ++i) {
		WorkerPool *workerPool = workerPools->at(i);
		ServiceInstance *serviceInstance = workerPool->serviceInstance();
		if (
			serviceInstance->host()->match(request->host()) ||
			serviceInstance->uri()->match(request->target())
		) {
			debug() << "Dispatched to " << serviceInstance->serviceName() << " service" << nl;
			workerPool->dispatch(client());
			close();
			return;
		}
	}
	warning() << "Failed to dispatch request to host = \"" << request->host() << "\", target = \"" << request->target() << "\"" << nl;
}

DispatchDelegate::DispatchDelegate(ServiceWorker *worker)
	: ServiceDelegate(worker)
{
	dispatchInstance_ = worker->serviceInstance();
}

} // namespace fnode
