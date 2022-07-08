/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/ForegroundLoggingService>
#include <cc/ForegroundLoggingInstance>
#include <cc/HttpLoggingServiceRegistry>
#include <cc/httpLogging>

namespace cc {

struct ForegroundLoggingService::State: public HttpLoggingService::State
{
    State():
        prototype_{ForegroundLoggingService::name()}
    {}

    HttpLoggingServiceConfigPrototype configPrototype() const override
    {
        return prototype_;
    }

    HttpLoggingServiceInstance createInstance(const MetaObject &config) const override
    {
        return ForegroundLoggingInstance{config};
    }

    HttpLoggingServiceConfigPrototype prototype_;
};

const char *ForegroundLoggingService::name()
{
    return foregroundLoggingServiceName();
}

ForegroundLoggingService::ForegroundLoggingService():
    HttpLoggingService{new State}
{}

class ForegroundLoggingServiceAnnouncer {
public:
    ForegroundLoggingServiceAnnouncer() {
        static bool done = false;
        if (done) return;
        HttpLoggingServiceRegistry{}.registerService(ForegroundLoggingService{});
        done = true;
    }
};

namespace { ForegroundLoggingServiceAnnouncer announcer; }

} // namespace cc
