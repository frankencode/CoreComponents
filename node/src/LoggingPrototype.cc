/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/http/LoggingPrototype>

namespace cc {
namespace http {

Ref<LoggingPrototype> LoggingPrototype::create(const String &className, const MetaProtocol *protocol)
{
    return new LoggingPrototype{className, protocol};
}

LoggingPrototype::LoggingPrototype(const String &className, const MetaProtocol *protocol):
    MetaPrototype{className, protocol}
{
    establish("verbosity", "");
}

}} // namespace cc::http
