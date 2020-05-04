/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/http/DispatchLoggingService>
#include <cc/http/DispatchLoggingInstance>
#include <cc/http/LoggingRegistry>

namespace cc {
namespace http {

Ref<DispatchLoggingService> DispatchLoggingService::create()
{
    return new DispatchLoggingService;
}

DispatchLoggingService::DispatchLoggingService():
    prototype_{LoggingPrototype::create("Dispatch", LoggingRegistry::instance()->loggingProtocol())}
{}

const LoggingPrototype *DispatchLoggingService::configPrototype() const
{
    return prototype_;
}

Ref<LoggingInstance> DispatchLoggingService::createInstance(const MetaObject *config) const
{
    return DispatchLoggingInstance::create(config);
}

class DispatchLoggingServiceAnnouncer {
public:
    DispatchLoggingServiceAnnouncer() {
        static bool done = false;
        if (done) return;
        LoggingRegistry::instance()->registerService(DispatchLoggingService::create());
        done = true;
    }
};

namespace { DispatchLoggingServiceAnnouncer announcer; }

}} // namespace cc::http
