/*
 * Copyright (C) 2017-2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Format>
#include <cc/ui/ScaledFont>

namespace cc {
namespace ui {

String ScaledFont::toString() const
{
    return Format()
        << "ScaledFont {\n"
        << "  family: " << family() << nl
        << "  size: " << size() << nl
        << "  weight: " << weight() << nl
        << "  slant: " << slant() << nl
        << "  stretch: " << stretch() << nl
        << "  pitch: " << pitch() << nl
        << "}";
}

}} // namespace cc::ui
