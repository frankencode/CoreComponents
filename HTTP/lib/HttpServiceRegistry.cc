/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/HttpServiceRegistry>
#include <cc/HttpServerConfigProtocol>

namespace cc {

struct HttpServiceRegistry::State: public Object::State
{
    Map<String, HttpService> serviceByName_;
};

HttpServiceRegistry::HttpServiceRegistry()
{
    initOnce<State>();
}

void HttpServiceRegistry::registerService(const HttpService &service)
{
    me().serviceByName_.establish(service.configPrototype().className(), service);
    HttpServerConfigProtocol{}.registerService(service.configPrototype());
}

HttpService HttpServiceRegistry::serviceByName(const String &name) const
{
    return me().serviceByName_(name);
}

const HttpServiceRegistry::State &HttpServiceRegistry::me() const
{
    return Object::me.as<State>();
}

HttpServiceRegistry::State &HttpServiceRegistry::me()
{
    return Object::me.as<State>();
}

} // namespace cc
