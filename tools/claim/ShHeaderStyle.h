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
    Ref<Header> scan(const String &path) const override;
    String str(const Notice *notice) const override;
    int magicCount(const String &text) const override;

private:
    friend class Object;

    ShHeaderStyle();
    Ref<SyntaxDefinition> headerSyntax_;
};

namespace { Registration<ShHeaderStyle> registration; }

} // namespace ccclaim
