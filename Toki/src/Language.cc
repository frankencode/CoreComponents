/*
 * Copyright (C) 2007-2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/toki/Language>
#include <cc/toki/Registry>

namespace cc::toki {

Language::Language(State *newState):
    Object{newState}
{
    Registry{}.registerLanguage(*this);
}

} // namespace cc::toki
