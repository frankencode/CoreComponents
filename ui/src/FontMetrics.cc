/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/FontMetrics>
#include <cc/ui/FontManager>
#include <cc/ui/Font>

namespace cc::ui {

FontMetrics::FontMetrics():
    Object{new State}
{}

FontMetrics::FontMetrics(const Font &font):
    Object{new State}
{
    *this = FontManager{}.selectFont(font).metrics();
}

} // namespace cc::ui
