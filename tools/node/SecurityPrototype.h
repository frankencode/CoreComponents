/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/meta/MetaObject>

namespace ccnode {

using namespace cc;
using namespace cc::meta;

class SecurityPrototype: public MetaObject
{
public:
    static Ref<SecurityPrototype> create() {
        return new SecurityPrototype;
    }

private:
    SecurityPrototype():
        MetaObject("TLS")
    {
        establish("certificate", "");
        establish("private-key", "");
        establish("ciphers", "");
        establish("session-resumption-key-refresh", 3600.);
    }
};

} // namespace ccnode
