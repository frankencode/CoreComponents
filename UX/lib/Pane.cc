/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/Pane>

namespace cc {

Size Pane::State::preferredSize() const
{
    return hasParent() ? parent().size() : View::State::preferredSize();
}

Pane &Pane::associate(Out<Pane> self)
{
    return View::associate<Pane>(self);
}

} // namespace cc
