/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Format>
#include <cc/ui/TextCursor>

namespace cc {
namespace ui {

String TextCursor::toString() const
{
    return Format()
        << "TextCursor {" << nl
        << "  posA: " << posA() << nl
        << "  posB: " << posB() << nl
        << "  advance: " << advance() << nl
        << "  byteOffset: " << byteOffset() << nl
        << "  isValid: " << isValid() << nl
        << "}";
}

}} // namespace cc::ui
