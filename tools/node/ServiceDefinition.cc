/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include "LogPrototype.h"
#include "SecurityPrototype.h"
#include "ServiceDefinition.h"

namespace ccnode {

Ref<ServicePrototype> ServicePrototype::create(const String &className, MetaProtocol *protocol)
{
    return new ServicePrototype{className, protocol};
}

ServicePrototype::ServicePrototype(const String &className, MetaProtocol *protocol):
    MetaObject(className, protocol)
{
    establish("request-limit", 100);
    establish("request-payload-limit", 0x10000);
    establish("host", "");
    establish("uri", "");
    establish("security", SecurityPrototype::create());
    establish("error-log", LogPrototype::create());
    establish("access-log", LogPrototype::create());
}

} // namespace ccnode
