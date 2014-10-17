/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include "ErrorLog.h"
#include "DispatchInstance.h"
#include "DispatchDelegate.h"

namespace fluxnode {

Ref<DispatchDelegate> DispatchDelegate::create(ServiceWorker *worker) {
    return new DispatchDelegate(worker);
}

void DispatchDelegate::process(Request *request)
{
    FLUXNODE_DEBUG() << "Dispatching request, host = \"" << request->host() << "\", target = \"" << request->target() << "\"..." << nl;
    WorkerPools *workerPools = dispatchInstance_->workerPools();
    for (int i = 0; i < workerPools->count(); ++i) {
        WorkerPool *workerPool = workerPools->at(i);
        ServiceInstance *serviceInstance = workerPool->serviceInstance();
        if (
            serviceInstance->host()->match(request->host())->valid() ||
            serviceInstance->uri()->match(request->target())->valid()
        ) {
            FLUXNODE_DEBUG() << "Dispatching to " << serviceInstance->serviceName() << " service..." << nl;
            client()->putBack(request);
            workerPool->dispatch(client());
            close();
            return;
        }
    }
    FLUXNODE_WARNING() << "Failed to dispatch request from " << client()->address() << ": host = \"" << request->host() << "\", target = \"" << request->target() << "\"" << nl;
}

DispatchDelegate::DispatchDelegate(ServiceWorker *worker)
    : ServiceDelegate(worker)
{
    dispatchInstance_ = worker->serviceInstance();
}

} // namespace fluxnode
