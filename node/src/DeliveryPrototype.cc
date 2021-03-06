/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/http/DeliveryPrototype>
#include <cc/http/LoggingRegistry>
#include <cc/http/SecurityPrototype>

namespace cc {
namespace http {

Ref<DeliveryPrototype> DeliveryPrototype::create(const String &className)
{
    return new DeliveryPrototype{className};
}

DeliveryPrototype::DeliveryPrototype(const String &className, const MetaProtocol *protocol):
    MetaPrototype{className, protocol}
{
    establish("request-limit", 100);
    establish("request-payload-limit", 0x10000);
    establish("host", "");
    establish("uri", "");
    establish("security", SecurityPrototype::create());
    establish("error-log", LoggingRegistry::instance()->loggingProtocol());
    establish("access-log", LoggingRegistry::instance()->loggingProtocol());
}

}} // namespace cc::http
