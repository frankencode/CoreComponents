/*
 * Copyright (C) 2025 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/TimeWebSocketService>
#include <cc/TimeWebSocketInstance>
#include <cc/HttpServiceRegistry>

namespace cc {

struct TimeWebSocketService::State: public HttpService::State
{
    State():
        configPrototype_{TimeWebSocketService::name()}
    {}

    HttpServiceConfigPrototype configPrototype() const override
    {
        return configPrototype_;
    }

    HttpServiceInstance createInstance(const MetaObject &config) const override
    {
        return TimeWebSocketInstance{config};
    }

    HttpServiceConfigPrototype configPrototype_;
};

const char *TimeWebSocketService::name()
{
    return "TimeWebSocket";
}

TimeWebSocketService::TimeWebSocketService():
    HttpService{new State}
{}

namespace { HttpServiceRegistry::Announcer<TimeWebSocketService> announcer; }

} // namespace cc
