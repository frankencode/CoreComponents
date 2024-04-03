/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/GlyphRun>
#include <cc/FontManager>

namespace cc {

GlyphRun::GlyphRun(const String &text, const Font &font, const Point &origin)
{
    *this = FontManager{}.typeset(text, font, origin);
}

} // namespace cc
