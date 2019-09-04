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

LoggingPrototype::LoggingPrototype(const String &name):
    MetaObject{name}
{
    establish("type", "");
    establish("level", "");
}

}} // namespace cc::node
