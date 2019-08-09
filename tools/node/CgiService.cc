/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include "ServiceRegistry.h"
#include "CgiInstance.h"
#include "CgiService.h"

namespace ccnode {

Ref<ServiceInstance> CgiService::createInstance(MetaObject *config) const
{
    return CgiInstance::create(config);
}

CgiService::CgiService():
    configPrototype_(ServicePrototype::create("CGI"))
{
    establish(configPrototype_);
}

void CgiService::establish(ServicePrototype *configPrototype)
{
    configPrototype->establish("script", "");
    configPrototype->establish("server", StringList::create());
    configPrototype->establish("environment", MetaObject::create());
}

class CgiServiceAnnouncer {
public:
    CgiServiceAnnouncer() {
        static bool done = false;
        if (done) return;
        Ref<CgiService> service = CgiService::create();
        ServiceRegistry::instance()->registerService(service);
        done = true;
    }
};

namespace { CgiServiceAnnouncer announcer; }

} // namespace ccnode
