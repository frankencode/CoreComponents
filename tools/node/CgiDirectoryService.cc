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

    virtual ServicePrototype *configPrototype() const { return configPrototype_; }

    virtual Ref<ServiceInstance> createInstance(MetaObject *config) const {
        return CgiDirectoryInstance::create(config);
    }

private:
    CgiDirectoryService():
        configPrototype_(ServicePrototype::create("CGI-Directory"))
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
        Ref<CgiDirectoryService> service = CgiDirectoryService::create();
        ServiceRegistry::instance()->registerService(service);
        done = true;
    }
};

namespace { CgiDirectoryServiceAnnouncer announcer; }

} // namespace ccnode
