/*
 * Copyright (C) 2017-2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/FontManager>
#include <cc/ui/GlyphRun>

namespace cc {
namespace ui {

Ref<GlyphRun> GlyphRun::typeSet(const String &text, const Font &font, const Point &origin)
{
    return FontManager::instance()->typeSet(text, font, origin);
}

}} // namespace cc::ui
