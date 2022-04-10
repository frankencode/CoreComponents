/*
 * Copyright (C) 2007-2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/claim/HeaderStyle>

namespace cc::claim {

String HeaderStyle::trimHeader(const String &text, const char *space)
{
    List<String> lines = text.split('\n');
    for (long i = 0; i < lines.count(); ++i) {
        lines[i].trim(space);
    }
    while (lines.count() > 0) {
        if (lines.first().count() == 0)
            lines.popFront();
        else
            break;
    }
    while (lines.count() > 0) {
        if (lines.last().count() == 0)
            lines.popBack();
        else
            break;
    }
    return lines.join('\n');
}

HeaderStyle::HeaderStyle(State *newState):
    Object{newState}
{
    /* TODO... */
}

} // namespace cc::claim
