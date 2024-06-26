/*
 * Copyright (C) 2007-2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/TokiLanguage>
#include <cc/TokiRegistry>

namespace cc {

TokiLanguage::TokiLanguage(State *newState):
    Object{newState}
{
    TokiRegistry{}.registerLanguage(*this);
}

} // namespace cc
