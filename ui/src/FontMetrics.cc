/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/FontMetrics>
#include <cc/FontManager>
#include <cc/Font>

namespace cc {

FontMetrics::FontMetrics():
    Object{new State}
{}

FontMetrics::FontMetrics(const Font &font):
    Object{new State}
{
    *this = FontManager{}.selectFont(font).metrics();
}

} // namespace cc
