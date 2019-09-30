/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/node/DispatchLoggingInstance>
#include <cc/node/LoggingRegistry>

namespace cc {
namespace node {

Ref<DispatchLoggingInstance> DispatchLoggingInstance::create(const MetaObject *config)
{
    return new DispatchLoggingInstance{config};
}

DispatchLoggingInstance::DispatchLoggingInstance(const MetaObject *config):
    LoggingInstance{config},
    loggingInstances_{LoggingInstances::create()}
{
    for (const MetaObject *child: config->children()) {
        const LoggingService *service = LoggingRegistry::instance()->serviceByName(child->className());
        loggingInstances_->append(service->createInstance(child));
    }
}

void DispatchLoggingInstance::logDelivery(const HttpServerConnection *client, int statusCode, size_t bytesWritten, const String &statusMessage) const
{
    for (const LoggingInstance *target: loggingInstances_)
        target->logDelivery(client, statusCode, bytesWritten, statusMessage);
}

void DispatchLoggingInstance::logMessage(const String &message, LoggingLevel level) const
{
    if (verbosity() >= level) {
        for (const LoggingInstance *target: loggingInstances_)
            target->logMessage(message, level);
    }
}

}} // namespace cc::node
