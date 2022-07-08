/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/DispatchLoggingService>
#include <cc/DispatchLoggingInstance>
#include <cc/HttpLoggingServiceRegistry>

namespace cc {

struct DispatchLoggingService::State: public HttpLoggingService::State
{
    State():
        prototype_{DispatchLoggingService::name(), HttpLoggingServiceRegistry{}.loggingProtocol()}
    {}

    HttpLoggingServiceConfigPrototype configPrototype() const override
    {
        return prototype_;
    }

    HttpLoggingServiceInstance createInstance(const MetaObject &config) const override
    {
        return DispatchLoggingInstance{config};
    }

    HttpLoggingServiceConfigPrototype prototype_;
};

const char *DispatchLoggingService::name()
{
    return "Dispatch";
}

DispatchLoggingService::DispatchLoggingService():
    HttpLoggingService{new State}
{}

class DispatchLoggingServiceAnnouncer {
public:
    DispatchLoggingServiceAnnouncer() {
        static bool done = false;
        if (done) return;
        HttpLoggingServiceRegistry{}.registerService(DispatchLoggingService{});
        done = true;
    }
};

namespace { DispatchLoggingServiceAnnouncer announcer; }

} // namespace cc
