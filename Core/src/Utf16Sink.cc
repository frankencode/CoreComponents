/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/Utf16Sink>

namespace cc {

String Utf16Sink::SurrogatePairEncodingError::message() const
{
    return "UTF-16 error: encoding surrogate pairs is not allowed (0xD800..0xDFFF)";
}

String Utf16Sink::ByteOrderMarkEncodingError::message() const
{
    return "UTF-16 error: encoding the BOM is not allowed (0xFEFF, 0xFFFE)";
}

String Utf16Sink::LargeCodePointEncodingError::message() const
{
    return "UTF-16 error: code point to large (>= 0x110000)";
}

} // namespace cc
