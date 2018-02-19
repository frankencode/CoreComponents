/*
 * Copyright (C) 2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/PlatformPlugin>
#include <cc/ui/TextRun>

namespace cc {
namespace ui {

Ref<TextRun> TextRun::create()
{
    return PlatformPlugin::instance()->createTypeSetter()->createTextRun();
}

}} // namespace cc::ui
