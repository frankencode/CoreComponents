/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include "Registry.h"
#include "HeaderStyle.h"

namespace fluxclaim {

HeaderStyle::HeaderStyle(String language)
    : language_(language)
{
    registry()->registerHeaderStyle(this);
}

String HeaderStyle::trimHeader(String text, const char *space)
{
    Ref<StringList> lines = text->split('\n');
    for (int i = 0; i < lines->count(); ++i)
        lines->at(i) = lines->at(i)->trim(space);
    while (lines->count() > 0) {
        if (lines->at(0)->count() == 0)
            lines->pop(0);
        else
            break;
    }
    while (lines->count() > 0) {
        if (lines->at(lines->count() - 1)->count() == 0)
            lines->pop(lines->count() - 1);
        else
            break;
    }
    return lines->join("\n");
}

} // namespace fluxclaim
