/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/http/LoggingRegistry>
#include <cc/Singleton>

namespace cc {
namespace http {

LoggingRegistry *LoggingRegistry::instance()
{
    return Singleton<LoggingRegistry>::instance();
}

LoggingRegistry::LoggingRegistry():
    loggingProtocol_{MetaProtocol::create()}
{}

void LoggingRegistry::registerService(LoggingService *service)
{
    serviceByName_->establish(service->configPrototype()->className(), service);
    loggingProtocol_->define(service->configPrototype());
}

LoggingService *LoggingRegistry::serviceByName(const String &name) const
{
    return serviceByName_->value(name);
}

MetaProtocol *LoggingRegistry::loggingProtocol() const
{
    return loggingProtocol_;
}

}} // namespace cc::http
