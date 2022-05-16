/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Icon>

namespace cc {

Icon::Icon(Ideographic ch, double size, Color color):
    Picture{ch, size, color ? color : theme().secondaryTextColor()}
{}

Icon::Icon(Ideographic ch, Color color):
    Picture{ch, sp(24), color}
{}

Icon &Icon::associate(Out<Icon> self)
{
    return View::associate<Icon>(self);
}

} // namespace cc
