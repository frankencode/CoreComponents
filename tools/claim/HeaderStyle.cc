/*
 * Copyright (C) 2007-2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/claim/HeaderStyle>
#include <cc/claim/Registry>

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
    Registry{}.registerHeaderStyle(*this);
}

} // namespace cc::claim
