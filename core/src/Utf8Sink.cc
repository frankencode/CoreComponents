/*
 * Copyright (C) 2007-2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Utf8Sink>

namespace cc {

String Utf8Sink::SurrogatePairEncodingError::message() const
{
    return "UTF-8 error: encoding of surrogate pairs is not allowed (0xD800..0xDFFF)";
}

String Utf8Sink::NonCharacterEncodingError::message() const
{
    return "UTF-8 error: encoding of non-characters is not allowed (0xFDD0..0xFFFE, 0x*FFF(E|F))";
}

String Utf8Sink::LargeCodePointEncodingError::message() const
{
    return "UTF-8 error: code point to large (>= 0x110000)";
}

} // namespace cc
