/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include "ServiceRegistry.h"
#include "ServiceDefinition.h"
#include "DirectoryInstance.h"

namespace fluxnode {

class DirectoryService: public ServiceDefinition
{
public:
    static Ref<DirectoryService> create() {
        return new DirectoryService;
    }

    virtual ServicePrototype *configPrototype() const { return configPrototype_; }
    virtual Ref<ServiceInstance> createInstance(MetaObject *config) const { return DirectoryInstance::create(config); }

private:
    DirectoryService():
        configPrototype_(ServicePrototype::create("Directory"))
    {
        configPrototype_->insert("path", "");
        configPrototype_->insert("show-hidden", false);
    }

    Ref<ServicePrototype> configPrototype_;
};

class DirectoryAnnouncer {
public:
    DirectoryAnnouncer() {
        static bool done = false;
        if (done) return;
        Ref<DirectoryService> directoryService = DirectoryService::create();
        serviceRegistry()->registerService(directoryService);
        done = true;
    }
};

namespace { DirectoryAnnouncer announcer; }

} // namespace fluxnode
