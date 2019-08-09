/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include "ServiceRegistry.h"
#include "DirectoryInstance.h"
#include "DirectoryService.h"

namespace ccnode {

Ref<ServiceInstance> DirectoryService::createInstance(MetaObject *config) const
{
    return DirectoryInstance::create(config);
}

DirectoryService::DirectoryService():
    configPrototype_{ServicePrototype::create("Directory")}
{
    establish(configPrototype_);
}

void DirectoryService::establish(ServicePrototype *configPrototype)
{
    configPrototype->establish("path", "");
    configPrototype->establish("show-hidden", false);
}

class DirectoryAnnouncer {
public:
    DirectoryAnnouncer() {
        static bool done = false;
        if (done) return;
        Ref<DirectoryService> directoryService = DirectoryService::create();
        ServiceRegistry::instance()->registerService(directoryService);
        done = true;
    }
};

namespace { DirectoryAnnouncer announcer; }

} // namespace ccnode
