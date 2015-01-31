/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXCLAIM_HEADERSTYLE_H
#define FLUXCLAIM_HEADERSTYLE_H

#include <flux/Registration>
#include <flux/List>
#include "Header.h"
#include "Notice.h"

namespace fluxclaim {

using namespace flux;

class HeaderStyle: public Object
{
public:
    inline String language() const { return language_; }

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

} // namespace fluxclaim

#endif // FLUXCLAIM_HEADERSTYLE_H
