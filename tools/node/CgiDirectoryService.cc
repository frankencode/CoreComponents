/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include "DeliveryRegistry.h"
#include "DeliveryService.h"
#include "DirectoryService.h"
#include "CgiService.h"
#include "CgiDirectoryInstance.h"

namespace ccnode {

class CgiDirectoryService: public DeliveryService
{
public:
    static Ref<CgiDirectoryService> create() {
        return new CgiDirectoryService;
    }

    DeliveryPrototype *configPrototype() const override { return configPrototype_; }

    Ref<DeliveryInstance> createInstance(const MetaObject *config) const override
    {
        return CgiDirectoryInstance::create(config);
    }

private:
    CgiDirectoryService():
        configPrototype_{DeliveryPrototype::create("CGI-Directory")}
    {
        CgiService::establish(configPrototype_);
        DirectoryService::establish(configPrototype_);
    }

    Ref<DeliveryPrototype> configPrototype_;
};

class CgiDirectoryServiceAnnouncer {
public:
    CgiDirectoryServiceAnnouncer() {
        static bool done = false;
        if (done) return;
        DeliveryRegistry::instance()->registerService<CgiDirectoryService>();
        done = true;
    }
};

namespace { CgiDirectoryServiceAnnouncer announcer; }

} // namespace ccnode
