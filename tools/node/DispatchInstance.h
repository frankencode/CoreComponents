/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXNODE_DISPATCHINSTANCE_H
#define FLUXNODE_DISPATCHINSTANCE_H

#include "ServiceInstance.h"
#include "WorkerPool.h"

namespace fluxnode {

class NodeMaster;

class DispatchInstance: public ServiceInstance
{
public:
    static Ref<DispatchInstance> create(MetaObject *config);
    virtual Ref<ServiceDelegate> createDelegate(ServiceWorker *worker) const;

    inline WorkerPools *workerPools() const { return workerPools_; }

private:
    friend class NodeMaster;

    DispatchInstance(MetaObject *config);

    Ref<WorkerPools> workerPools_;
};

} // namespace fluxnode

#endif // FLUXNODE_DISPATCHINSTANCE_H
