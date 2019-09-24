/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/node/SystemLoggingInstance>
#include <syslog.h>

namespace cc {
namespace node {

Ref<SystemLoggingInstance> SystemLoggingInstance::create(const MetaObject *config)
{
    return new SystemLoggingInstance{config};
}

SystemLoggingInstance::SystemLoggingInstance(const MetaObject *config):
    LoggingInstance{config}
{}

int SystemLoggingInstance::getSyslogPriority(LoggingLevel level)
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

void SystemLoggingInstance::logDelivery(const HttpServerConnection *client, int statusCode, size_t bytesWritten, const String &statusMessage) const
{
    syslog(
        LOG_NOTICE,
        "%s",
        formatDeliveryLogMessage(client, statusCode, bytesWritten, statusMessage)
    );
}

void SystemLoggingInstance::logStatus(const String &message, LoggingLevel level) const
{
    if (verbosity() >= level)
        syslog(getSyslogPriority(level), "%s", message->chars());
}

}} // namespace cc::node
