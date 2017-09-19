/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/Application>
#include <cc/ui/FontMetrics>

namespace cc {
namespace ui {

Ref<FontMetrics> FontMetrics::get(Font *font)
{
    return Application::instance()->fontProvider()->getMetrics(font);
}

}} // namespace cc::ui
