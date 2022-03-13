/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/GlyphRun>
#include <cc/ui/FontManager>

namespace cc::ui {

GlyphRun::GlyphRun(const String &text, const Font &font, const Point &origin)
{
    *this = FontManager{}.typeset(text, font, origin);
}

} // namespace cc::ui
