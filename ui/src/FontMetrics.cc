/*
 * Copyright (C) 2017-2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/format>
#include <cc/ui/FontMetrics>

namespace cc {
namespace ui {

string FontMetrics::toString() const
{
    return format{}
        << "FontMetrics {\n"
        << "  fontSize: " << fontSize() << nl
        << "  ascender: " << ascender() << nl
        << "  descender: " << descender() << nl
        << "  lineHeight: " << lineHeight() << nl
        << "  underlinePosition: " << underlinePosition() << nl
        << "  underlineThickness: " << underlineThickness() << nl
        << "}";
}

}} // namespace cc::ui
