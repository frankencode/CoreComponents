/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/node/ForegroundLoggingInstance>
#include <cc/stdio>

namespace cc {
namespace node {

Ref<ForegroundLoggingInstance> ForegroundLoggingInstance::create(const MetaObject *config)
{
    return new ForegroundLoggingInstance{config};
}

ForegroundLoggingInstance::ForegroundLoggingInstance(const MetaObject *config):
    LoggingInstance{config}
{}

void ForegroundLoggingInstance::logDelivery(const HttpServerConnection *client, int statusCode, size_t bytesWritten, const String &statusMessage) const
{
    (statusCode < 400 ? stdOut() : stdErr())->write(
        formatDeliveryLogMessage(client, statusCode, bytesWritten, statusMessage)
    );
}

void ForegroundLoggingInstance::logStatus(const String &message, LoggingLevel level) const
{
    if (verbosity() >= level)
        stdErr()->write(message);
}

}} // namespace cc::node
