/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/node/LoggingPrototype>

namespace cc {
namespace node {

Ref<LoggingPrototype> LoggingPrototype::create(const String &className)
{
    return new LoggingPrototype{className};
}

LoggingPrototype::LoggingPrototype(const String &className):
    MetaPrototype{className}
{
    establish("type", "");
    establish("level", "");
}

}} // namespace cc::node
