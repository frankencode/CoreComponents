/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include "HeaderStyle.h"
#include "Registry.h"

namespace ccclaim {

HeaderStyle::HeaderStyle(const string &language):
    language_{language}
{
    registry()->registerHeaderStyle(this);
}

string HeaderStyle::trimHeader(const string &text, const char *space)
{
    Ref<StringList> lines = text->split('\n');
    for (int i = 0; i < lines->count(); ++i)
        lines->at(i) = lines->at(i)->trim(space);
    while (lines->count() > 0) {
        if (lines->at(0)->count() == 0)
            lines->removeAt(0);
        else
            break;
    }
    while (lines->count() > 0) {
        if (lines->at(lines->count() - 1)->count() == 0)
            lines->removeAt(lines->count() - 1);
        else
            break;
    }
    return lines->join("\n");
}

} // namespace ccclaim
