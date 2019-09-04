/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Registration>
#include <cc/List>
#include "Header.h"
#include "Notice.h"

namespace ccclaim {

using namespace cc;

class HeaderStyle: public Object
{
public:
    String language() const { return language_; }

    virtual Ref<Header> scan(String path) const = 0;
    virtual String str(Notice *notice) const = 0;
    virtual int magicCount(String text) const { return 0; }

protected:
    static String trimHeader(String text, const char *space);

    HeaderStyle(String language);

    Ref<HeaderStyle> headerStyle_;
    String language_;
};

typedef List< Ref<HeaderStyle> > HeaderStyleList;

} // namespace ccclaim
