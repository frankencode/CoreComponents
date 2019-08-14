/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include "ServiceRegistry.h"
#include "ServiceDefinition.h"
#include "EchoInstance.h"

namespace ccnode {

class EchoService: public ServiceDefinition
{
public:
    static Ref<EchoService> create() {
        return new EchoService;
    }

    ServicePrototype *configPrototype() const override { return configPrototype_; }
    Ref<ServiceInstance> createInstance(const MetaObject *config) const override { return EchoInstance::create(config); }

private:
    EchoService():
        configPrototype_{ServicePrototype::create("Echo")}
    {}

    Ref<ServicePrototype> configPrototype_;
};

class EchoAnnouncer {
public:
    EchoAnnouncer()
    {
        static bool done = false;
        if (done) return;
        ServiceRegistry::instance()->registerService<EchoService>();
        done = true;
    }
};

namespace { EchoAnnouncer announcer; }

} // namespace ccnode
