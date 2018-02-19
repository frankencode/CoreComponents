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

Ref<GlyphRun> GlyphRun::layout(String text, const TextStyle *style)
{
    return PlatformPlugin::instance()->createTypeSetter()->layout(text, style);
}

}} // namespace cc::ui
