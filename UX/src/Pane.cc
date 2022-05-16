/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
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
