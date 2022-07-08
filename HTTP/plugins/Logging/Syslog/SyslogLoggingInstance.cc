/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/SyslogLoggingInstance>
#include <cc/httpLogging>
#include <syslog.h>

namespace cc {

struct SyslogLoggingInstance::State: public HttpLoggingServiceInstance::State
{
    State(const MetaObject &config):
        HttpLoggingServiceInstance::State{config}
    {}

    void logDelivery(const HttpClientConnection &client, const HttpRequest &request, HttpStatus status, long long bytesWritten, const String &statusMessage) const override
    {
        String message = formatDeliveryNotice(client, request, status, bytesWritten, statusMessage);
        ::syslog(LOG_NOTICE, "%s", message.chars());
    }

    void logMessage(const String &message, LoggingLevel level) const override
    {
        if (verbosity() >= level) {
            ::syslog(getSyslogPriority(level), "%s", message.chars());
        }
    }

    static int getSyslogPriority(LoggingLevel level)
    {
        int priority = LOG_DEBUG;
        switch (level) {
            case LoggingLevel::Silent :
            case LoggingLevel::Debug  : priority = LOG_DEBUG  ; break;
            case LoggingLevel::Info   : priority = LOG_INFO   ; break;
            case LoggingLevel::Notice : priority = LOG_NOTICE ; break;
            case LoggingLevel::Warning: priority = LOG_WARNING; break;
            case LoggingLevel::Error  : priority = LOG_ERR    ; break;
        };
        return priority;
    }
};

SyslogLoggingInstance::SyslogLoggingInstance(const MetaObject &config):
    HttpLoggingServiceInstance{new State{config}}
{}

} // namespace cc
