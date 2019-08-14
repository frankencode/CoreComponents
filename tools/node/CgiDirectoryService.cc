/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include "ServiceRegistry.h"
#include "ServiceDefinition.h"
#include "DirectoryService.h"
#include "CgiService.h"
#include "CgiDirectoryInstance.h"

namespace ccnode {

class CgiDirectoryService: public ServiceDefinition
{
public:
    static Ref<CgiDirectoryService> create() {
        return new CgiDirectoryService;
    }

    ServicePrototype *configPrototype() const override { return configPrototype_; }

    Ref<ServiceInstance> createInstance(const MetaObject *config) const override
    {
        return CgiDirectoryInstance::create(config);
    }

private:
    CgiDirectoryService():
        configPrototype_{ServicePrototype::create("CGI-Directory")}
    {
        CgiService::establish(configPrototype_);
        DirectoryService::establish(configPrototype_);
    }

    Ref<ServicePrototype> configPrototype_;
};

class CgiDirectoryServiceAnnouncer {
public:
    CgiDirectoryServiceAnnouncer() {
        static bool done = false;
        if (done) return;
        ServiceRegistry::instance()->registerService<CgiDirectoryService>();
        done = true;
    }
};

namespace { CgiDirectoryServiceAnnouncer announcer; }

} // namespace ccnode
