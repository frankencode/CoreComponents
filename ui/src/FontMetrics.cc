/*
 * Copyright (C) 2017-2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Format>
#include <cc/ui/FontMetrics>

namespace cc {
namespace ui {

String FontMetrics::toString() const
{
    return Format()
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
