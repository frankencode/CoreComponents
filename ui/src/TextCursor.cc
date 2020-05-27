/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/format>
#include <cc/ui/TextCursor>

namespace cc {
namespace ui {

string TextCursor::toString() const
{
    return format{}
        << "TextCursor {" << nl
        << "  posA: " << posA() << nl
        << "  posB: " << posB() << nl
        << "  advance: " << advance() << nl
        << "  byteOffset: " << byteOffset() << nl
        << "  isValid: " << isValid() << nl
        << "}";
}

}} // namespace cc::ui
