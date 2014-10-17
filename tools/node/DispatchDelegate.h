/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXNODE_DISPATCHDELEGATE_H
#define FLUXNODE_DISPATCHDELEGATE_H

#include "ServiceDelegate.h"
#include "WorkerPool.h"

namespace fluxnode
{

class DispatchInstance;

class DispatchDelegate: public ServiceDelegate
{
public:
    static Ref<DispatchDelegate> create(ServiceWorker *worker);

    virtual void process(Request *request);

private:
    DispatchDelegate(ServiceWorker *worker);

    Ref<DispatchInstance> dispatchInstance_;
};

} // namespace fluxnode

#endif // FLUXNODE_DISPATCHDELEGATE_H
