/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include "DeliveryRegistry.h"
#include "DeliveryService.h"
#include "EchoInstance.h"

namespace ccnode {

class EchoService: public DeliveryService
{
public:
    static Ref<EchoService> create() {
        return new EchoService;
    }

    DeliveryPrototype *configPrototype() const override { return configPrototype_; }
    Ref<DeliveryInstance> createInstance(const MetaObject *config) const override { return EchoInstance::create(config); }

private:
    EchoService():
        configPrototype_{DeliveryPrototype::create("Echo")}
    {}

    Ref<DeliveryPrototype> configPrototype_;
};

class EchoAnnouncer {
public:
    EchoAnnouncer()
    {
        static bool done = false;
        if (done) return;
        DeliveryRegistry::instance()->registerService<EchoService>();
        done = true;
    }
};

namespace { EchoAnnouncer announcer; }

} // namespace ccnode
