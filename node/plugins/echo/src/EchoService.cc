/*
 * Copyright (C) 2007-2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/http/EchoService>
#include <cc/http/DeliveryRegistry>
#include <cc/http/DeliveryService>
#include <cc/http/EchoInstance>

namespace cc {
namespace http {

const char *EchoService::name()
{
    return "Echo";
}

Ref<EchoService> EchoService::create()
{
    return new EchoService;
}

DeliveryPrototype *EchoService::configPrototype() const
{
    return configPrototype_;
}

Ref<DeliveryInstance> EchoService::createInstance(const MetaObject *config) const
{
    return EchoInstance::create(config);
}

EchoService::EchoService():
    configPrototype_{DeliveryPrototype::create(name())}
{}

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

}} // namespace cc::http
