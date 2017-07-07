/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Date>
#include <cc/meta/MetaObject>

namespace ccnode {

using namespace cc;
using namespace cc::meta;

class LogPrototype: public MetaObject
{
public:
    static Ref<LogPrototype> create() { return new LogPrototype; }

private:
    LogPrototype():
        MetaObject("Log")
    {
        establish("path", "");
        establish("level", "");
        establish("retention", days(30));
        establish("rotation", days(1));
    }
};

} // namespace ccnode
