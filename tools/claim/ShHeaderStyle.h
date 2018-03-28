/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include "HeaderStyle.h"

namespace ccclaim {

using namespace cc;

class ShHeaderStyle: public HeaderStyle
{
public:
    virtual Ref<Header> scan(String path) const;
    virtual String str(Notice *notice) const;
    virtual int magicCount(String text) const;

private:
    friend class Object;

    ShHeaderStyle();
    Ref<SyntaxDefinition> headerSyntax_;
};

namespace { Registration<ShHeaderStyle> registration; }

} // namespace ccclaim
