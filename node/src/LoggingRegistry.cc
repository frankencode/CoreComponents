/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/node/LoggingRegistry>
#include <cc/Singleton>
#include <cc/Map>

namespace cc {
namespace node {

LoggingRegistry *LoggingRegistry::instance()
{
    return Singleton<LoggingRegistry>::instance();
}

LoggingRegistry::LoggingRegistry():
    serviceByName_{ServiceByName::create()},
    loggingProtocol_{MetaProtocol::create()}
{}

void LoggingRegistry::registerService(LoggingService *service)
{
    serviceByName_->establish(service->configPrototype()->className(), service);
    loggingProtocol_->define(service->configPrototype());
}

MetaProtocol *LoggingRegistry::loggingProtocol() const
{
    return loggingProtocol_;
}

}} // namespace cc::node
