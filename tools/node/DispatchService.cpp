/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include "ServiceRegistry.h"
#include "ServiceDefinition.h"
#include "WorkerPool.h"
#include "DispatchInstance.h"

namespace fluxnode
{

class DispatchService: public ServiceDefinition
{
public:
    static Ref<DispatchService> create() {
        return new DispatchService;
    }

    virtual ServicePrototype *configPrototype() const { return configPrototype_; }
    virtual Ref<ServiceInstance> createInstance(MetaObject *config) const { return DispatchInstance::create(config); }

private:
    DispatchService()
        : configPrototype_(ServicePrototype::create("Dispatch"))
    {
        configPrototype_->remove("connection_timeout");
    }

    Ref<ServicePrototype> configPrototype_;
};

class DispatchAnnouncer {
public:
    DispatchAnnouncer()
    {
        static bool done = false;
        if (done) return;
        Ref<DispatchService> dispatchService = DispatchService::create();
        serviceRegistry()->registerService(dispatchService);
        done = true;
    }
};

namespace { DispatchAnnouncer announcer; }

} // namespace fluxnode
