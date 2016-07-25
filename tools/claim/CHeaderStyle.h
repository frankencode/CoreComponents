/*
 * Copyright (C) 2007-2016 Frank Mertens.
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
    virtual Ref<Header> scan(String path) const;
    virtual String str(Notice *notice) const;

private:
    friend class Registration<CHeaderStyle>;

    CHeaderStyle();
    Ref<SyntaxDefinition> headerSyntax_;
};

namespace { Registration<CHeaderStyle> registration; }

} // namespace ccclaim

