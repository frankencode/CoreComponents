/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/node/DeliveryPrototype>
#include <cc/node/LogPrototype>
#include <cc/node/SecurityPrototype>

namespace cc {
namespace node {

Ref<DeliveryPrototype> DeliveryPrototype::create(const String &className)
{
    return new DeliveryPrototype{className};
}

DeliveryPrototype::DeliveryPrototype(const String &className):
    MetaPrototype{className}
{
    establish("request-limit", 100);
    establish("request-payload-limit", 0x10000);
    establish("host", "");
    establish("uri", "");
    establish("security", SecurityPrototype::create());
    establish("error-log", LogPrototype::create());
    establish("access-log", LogPrototype::create());
}

}} // namespace cc::node
