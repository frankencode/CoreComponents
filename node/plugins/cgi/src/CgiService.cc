/*
 * Copyright (C) 2007-2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/node/CgiService>
#include <cc/node/DeliveryRegistry>
#include <cc/node/CgiInstance>

namespace cc {
namespace node {

Ref<CgiService> CgiService::create()
{
    return new CgiService;
}

Ref<DeliveryInstance> CgiService::createInstance(const MetaObject *config) const
{
    return CgiInstance::create(config);
}

CgiService::CgiService():
    configPrototype_{DeliveryPrototype::create("CGI")}
{
    establish(configPrototype_);
}

void CgiService::establish(DeliveryPrototype *configPrototype)
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
        DeliveryRegistry::instance()->registerService<CgiService>();
        done = true;
    }
};

namespace { CgiServiceAnnouncer announcer; }

}} // namespace cc::node
