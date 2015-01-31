/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXCLAIM_CHEADERSTYLE_H
#define FLUXCLAIM_CHEADERSTYLE_H

#include "HeaderStyle.h"

namespace fluxclaim {

using namespace flux;

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

} // namespace fluxclaim

#endif // FLUXCLAIM_CHEADERSTYLE_H
