/*
 * Copyright (C) 2017-2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/PlatformPlugin>
#include <cc/ui/GlyphRun>

namespace cc {
namespace ui {

Ref<GlyphRun> GlyphRun::layout(String text, const Font &font)
{
    return PlatformPlugin::instance()->createTypeSetter()->layout(text, font);
}

}} // namespace cc::ui
