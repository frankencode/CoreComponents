/*
 * Copyright (C) 2017-2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Format>
#include <cc/ui/TextStyle>

namespace cc {
namespace ui {

String TextStyle::toString() const
{
    return Format()
        << "TextStyle {" << nl
        << "  font: " << font()->toString()->replace("\n", "\n  ") << nl
        << "  color: " << color() << nl
        << "  decoration: " << decoration() << nl
        << "}";
}

}} // namespace cc::ui
