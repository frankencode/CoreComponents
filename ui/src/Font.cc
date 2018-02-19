/*
 * Copyright (C) 2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

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

}} // namespace cc::ui
