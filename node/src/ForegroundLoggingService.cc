/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/node/ForegroundLoggingService>
#include <cc/node/ForegroundLoggingInstance>
#include <cc/node/LoggingRegistry>

namespace cc {
namespace node {

Ref<ForegroundLoggingService> ForegroundLoggingService::create()
{
    return new ForegroundLoggingService;
}

ForegroundLoggingService::ForegroundLoggingService():
    prototype_{LoggingPrototype::create("Foreground-Log")}
{}

const LoggingPrototype *ForegroundLoggingService::configPrototype() const
{
    return prototype_;
}

Ref<LoggingInstance> ForegroundLoggingService::createInstance(const MetaObject *config) const
{
    return ForegroundLoggingInstance::create(config);
}

class ForegroundLoggingServiceAnnouncer {
public:
    ForegroundLoggingServiceAnnouncer() {
        static bool done = false;
        if (done) return;
        LoggingRegistry::instance()->registerService(ForegroundLoggingService::create());
        done = true;
    }
};

namespace { ForegroundLoggingServiceAnnouncer announcer; }

}} // namespace cc::node
