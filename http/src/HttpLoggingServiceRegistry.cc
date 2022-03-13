/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/HttpLoggingServiceRegistry>
#include <cc/Map>

namespace cc {

struct HttpLoggingServiceRegistry::State: public Object::State
{
    Map<String, HttpLoggingService> serviceByName_;
    MetaProtocol loggingProtocol_;
};

HttpLoggingServiceRegistry::HttpLoggingServiceRegistry()
{
    initOnce<State>();
}

void HttpLoggingServiceRegistry::registerService(const HttpLoggingService &service)
{
    me().serviceByName_.establish(service.configPrototype().className(), service);
    me().loggingProtocol_.define(service.configPrototype());
}

HttpLoggingService HttpLoggingServiceRegistry::serviceByName(const String &name) const
{
    return me().serviceByName_(name);
}

MetaProtocol HttpLoggingServiceRegistry::loggingProtocol() const
{
    return me().loggingProtocol_;
}

HttpLoggingServiceRegistry::State &HttpLoggingServiceRegistry::me()
{
    return Object::me.as<State>();
}

const HttpLoggingServiceRegistry::State &HttpLoggingServiceRegistry::me() const
{
    return Object::me.as<State>();
}

} // namespace cc
