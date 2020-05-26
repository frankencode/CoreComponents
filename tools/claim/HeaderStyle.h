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
    string language() const { return language_; }

    virtual Ref<Header> scan(const string &path) const = 0;
    virtual string str(const Notice *notice) const = 0;
    virtual int magicCount(const string &text) const { return 0; }

protected:
    static string trimHeader(const string &text, const char *space);

    HeaderStyle(const string &language);

    Ref<HeaderStyle> headerStyle_;
    string language_;
};

typedef List< Ref<HeaderStyle> > HeaderStyleList;

} // namespace ccclaim
