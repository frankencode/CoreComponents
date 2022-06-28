/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Font>
#include <cc/FontManager>
#include <cc/StylePlugin>
#include <cc/Format>

namespace cc {

Font::Font(Pitch pitch)
{
    *this = (pitch == Pitch::Fixed) ? style().defaultFixedFont() : style().defaultFont();
}

double Font::defaultSize(Pitch pitch)
{
    return ((pitch == Pitch::Fixed) ? style().defaultFixedFont() : style().defaultFont()).size();
}

String Font::path() const
{
    return FontManager{}.selectFont(*this).fontFace().path();
}

String str(const Font &font)
{
    return Format{}
        << "Font {" << nl
        << "  family: " << font.family() << nl
        << "  size: " << font.size() << nl
        << "  weight: " << font.weight() << nl
        << "  slant: " << font.slant() << nl
        << "  stretch: " << font.stretch() << nl
        << "  decoration: " << font.decoration() << nl
        << "  smoothing: " << font.smoothing() << nl
        << "  outlineHinting: " << font.outlineHinting() << nl
        << "  metricsHinting: " << font.metricsHinting() << nl
        << "  color: " << font.color() << nl
        << "  paper: " << font.paper() << nl
        << "}";
}

} // namespace cc
