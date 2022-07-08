/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/SyslogLoggingService>
#include <cc/SyslogLoggingInstance>
#include <cc/HttpLoggingServiceRegistry>
#include <cc/httpLogging>
#include <syslog.h>

namespace cc {

struct SyslogLoggingService::State: public HttpLoggingService::State
{
    State():
        prototype_{SyslogLoggingService::name()}
    {
        prototype_.establish("daemon-name", "cc_http");
    }

    HttpLoggingServiceConfigPrototype configPrototype() const override
    {
        return prototype_;
    }

    HttpLoggingServiceInstance createInstance(const MetaObject &config) const override
    {
        const String serviceName = config("daemon-name").to<String>();
        ::openlog(serviceName.chars(), 0, LOG_DAEMON);
        return SyslogLoggingInstance{config};
    }

    HttpLoggingServiceConfigPrototype prototype_;
};

const char *SyslogLoggingService::name()
{
    return syslogLoggingServiceName();
}

SyslogLoggingService::SyslogLoggingService():
    HttpLoggingService{new State}
{}

class SyslogLoggingServiceAnnouncer {
public:
    SyslogLoggingServiceAnnouncer() {
        static bool done = false;
        if (done) return;
        HttpLoggingServiceRegistry{}.registerService(SyslogLoggingService{});
        done = true;
    }
};

namespace { SyslogLoggingServiceAnnouncer announcer; }

} // namespace cc
