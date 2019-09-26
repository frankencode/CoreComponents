/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/node/SystemLoggingService>
#include <cc/node/SystemLoggingInstance>
#include <cc/node/LoggingRegistry>
#include <cc/node/SystemLogStream>
#include <cc/NullStream>
#include <syslog.h>

namespace cc {
namespace node {

void SystemLoggingService::open(const String &daemonName)
{
    ::openlog(daemonName, 0, LOG_DAEMON);
}

Ref<SystemLoggingService> SystemLoggingService::create()
{
    return new SystemLoggingService;
}

SystemLoggingService::SystemLoggingService():
    prototype_{LoggingPrototype::create("System-Log")}
{}

const LoggingPrototype *SystemLoggingService::configPrototype() const
{
    return prototype_;
}

Ref<LoggingInstance> SystemLoggingService::createInstance(const MetaObject *config) const
{
    return SystemLoggingInstance::create(config);
}

class SystemLoggingServiceAnnouncer {
public:
    SystemLoggingServiceAnnouncer() {
        static bool done = false;
        if (done) return;
        LoggingRegistry::instance()->registerService(SystemLoggingService::create());
        done = true;
    }
};

namespace { SystemLoggingServiceAnnouncer announcer; }

}} // namespace cc::node
