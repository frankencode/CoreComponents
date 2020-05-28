/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Format>
#include <cc/ui/Display>

namespace cc {
namespace ui {

String Display::toString() const
{
    return Format{}
        << "Display {" << nl
        << "  pos       : " << size() << nl
        << "  size      : " << pos() << nl
        << "  dpi       : " << dpi() << nl
        << "  diagonal  : " << diagonal() << nl
        << "  isHandheld: " << isHandheld() << nl
        << "}";
}

}} // namespace cc::ui
