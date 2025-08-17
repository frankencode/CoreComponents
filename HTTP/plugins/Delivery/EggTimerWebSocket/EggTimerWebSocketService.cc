/*
 * Copyright (C) 2025 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/EggTimerWebSocketService>
#include <cc/EggTimerWebSocketInstance>
#include <cc/HttpServiceRegistry>

namespace cc {

struct EggTimerWebSocketService::State: public HttpService::State
{
    State():
        configPrototype_{EggTimerWebSocketService::name()}
    {}

    HttpServiceConfigPrototype configPrototype() const override
    {
        return configPrototype_;
    }

    HttpServiceInstance createInstance(const MetaObject &config) const override
    {
        return EggTimerWebSocketInstance{config};
    }

    HttpServiceConfigPrototype configPrototype_;
};

const char *EggTimerWebSocketService::name()
{
    return "EggTimerWebSocket";
}

EggTimerWebSocketService::EggTimerWebSocketService():
    HttpService{new State}
{}

namespace { HttpServiceRegistry::Announcer<EggTimerWebSocketService> announcer; }

} // namespace cc
