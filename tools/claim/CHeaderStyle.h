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

class CHeaderStyle: public HeaderStyle
{
public:
    Ref<Header> scan(const String &path) const override;
    String str(const Notice *notice) const override;

private:
    friend class Object;

    CHeaderStyle();
    Ref<SyntaxDefinition> headerSyntax_;
};

namespace { Registration<CHeaderStyle> registration; }

} // namespace ccclaim
