/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Font>
#include <cc/StylePlugin>

namespace cc {

Font::Font(Pitch pitch)
{
    *this = (pitch == Pitch::Fixed) ? style().defaultFixedFont() : style().defaultFont();
}

double Font::defaultSize(Pitch pitch)
{
    return ((pitch == Pitch::Fixed) ? style().defaultFixedFont() : style().defaultFont()).size();
}

} // namespace cc
