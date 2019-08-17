/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include "LogPrototype.h"
#include "SecurityPrototype.h"
#include "WebService.h"

namespace ccnode {

Ref<WebServicePrototype> WebServicePrototype::create(const String &className)
{
    return new WebServicePrototype{className};
}

WebServicePrototype::WebServicePrototype(const String &className):
    MetaObject{className}
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
