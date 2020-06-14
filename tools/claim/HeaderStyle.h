/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Registration>
#include "Header.h"
#include "Notice.h"

namespace ccclaim {

using namespace cc;

class HeaderStyle: public Object
{
public:
    String language() const { return language_; }

    virtual Ref<Header> scan(const String &path) const = 0;
    virtual String str(const Notice *notice) const = 0;
    virtual int magicCount(const String &text) const { return 0; }

protected:
    static String trimHeader(const String &text, const char *space);

    HeaderStyle(const String &language);

    Ref<HeaderStyle> headerStyle_;
    String language_;
};

} // namespace ccclaim
