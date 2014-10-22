/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include "DispatchDelegate.h"
#include "DispatchInstance.h"

namespace fluxnode {

Ref<DispatchInstance> DispatchInstance::create(MetaObject *config) {
    return new DispatchInstance(config);
}

Ref<ServiceDelegate> DispatchInstance::createDelegate(ServiceWorker *worker) const {
    return DispatchDelegate::create(worker);
}

DispatchInstance::DispatchInstance(MetaObject *config):
    ServiceInstance(config)
{}

} // namespace fluxnode
