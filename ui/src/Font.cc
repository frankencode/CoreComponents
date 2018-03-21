/*
 * Copyright (C) 2017-2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Format>
#include <cc/ui/FontManager>
#include <cc/ui/Font>

namespace cc {
namespace ui {

Ref<Font> Font::select(
    String family,
    double size,
    Weight weight,
    Slant slant,
    Stretch stretch
) {
    return FontManager::instance()->selectFont(family, size, weight, slant, stretch);
}

String Font::toString() const
{
    return Format()
        << "Font {" << nl
        << "  family: " << family() << nl
        << "  size: " << size() << nl
        << "  weight: " << weight() << nl
        << "  slant: " << slant() << nl
        << "  stretch: " << stretch() << nl
        << "}";
}

}} // namespace cc::ui
