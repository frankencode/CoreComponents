/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/EchoService>
#include <cc/EchoInstance>
#include <cc/HttpServiceRegistry>

namespace cc {

struct EchoService::State: public HttpService::State
{
    State():
        configPrototype_{EchoService::name()}
    {}

    HttpServiceConfigPrototype configPrototype() const override
    {
        return configPrototype_;
    }

    HttpServiceInstance createInstance(const MetaObject &config) const override
    {
        return EchoInstance{config};
    }

    HttpServiceConfigPrototype configPrototype_;
};

const char *EchoService::name()
{
    return "Echo";
}

EchoService::EchoService():
    HttpService{new State}
{}

namespace { HttpServiceRegistry::Announcer<EchoService> announcer; }

} // namespace cc
