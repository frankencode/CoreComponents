/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXNODE_WORKERPOOL_H
#define FLUXNODE_WORKERPOOL_H

#include "ServiceWorker.h"

namespace fluxnode
{

class WorkerPool: public Object
{
public:
    static Ref<WorkerPool> create(ServiceInstance *serviceInstance, ClosedConnections *closedConnections);

    inline ServiceInstance *serviceInstance() const { return serviceInstance_; }

    void dispatch(ClientConnection *client);

private:
    WorkerPool(ServiceInstance *serviceInstance, ClosedConnections *closedConnections);
    ~WorkerPool();

    typedef Array< Ref<ServiceWorker> > ServiceWorkers;

    Ref<ClosedConnections> closedConnections_;
    Ref<ServiceInstance> serviceInstance_;
    Ref<ServiceWorkers> serviceWorkers_;
};

typedef Array< Ref<WorkerPool> > WorkerPools;

} // namespace fluxnode

#endif // FLUXNODE_WORKERPOOL_H
