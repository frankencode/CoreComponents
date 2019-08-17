/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include "DeliveryRegistry.h"
#include "DirectoryInstance.h"
#include "DirectoryService.h"

namespace ccnode {

Ref<DeliveryInstance> DirectoryService::createInstance(const MetaObject *config) const
{
    return DirectoryInstance::create(config);
}

DirectoryService::DirectoryService():
    configPrototype_{DeliveryPrototype::create("Directory")}
{
    establish(configPrototype_);
}

void DirectoryService::establish(DeliveryPrototype *configPrototype)
{
    configPrototype->establish("path", "");
    configPrototype->establish("show-hidden", false);
}

class DirectoryAnnouncer {
public:
    DirectoryAnnouncer() {
        static bool done = false;
        if (done) return;
        DeliveryRegistry::instance()->registerService<DirectoryService>();
        done = true;
    }
};

namespace { DirectoryAnnouncer announcer; }

} // namespace ccnode
