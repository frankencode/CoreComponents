/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/Utf8Sink>

namespace cc {

void Utf8Sink::write(char32_t ch)
{
    if (ch < 0x80) { // ASCII range: 0xxxxxxx
        byteSink_.writeUInt8(ch);
    }
    else if (ch < 0x800) { // two-byte codes: 110yyyxx | 10xxxxx
        byteSink_.writeUInt8((ch >> 6) | 0xC0);   // 0xC = (1100)2, code prefix: (110)2
        byteSink_.writeUInt8((ch & 0x3F) | 0x80); // 0x8 = (1000)2, code prefix: (10)2
    }
    else if (ch < 0x10000) { // three-byte codes: 1110yyyy | 10yyyyxx | 10xxxxxx
        if ((0xD800 <= ch) && (ch <= 0xDFFF)) throw SurrogatePairEncodingError{};
        else if ((0xFDD0 <= ch) && (ch <= 0xFDEF)) throw NonCharacterEncodingError{};
        byteSink_.writeUInt8((ch >> 12) | 0xE0);         // 0xE = (1110)2, code prefix: (1110)2
        byteSink_.writeUInt8(((ch >> 6) & 0x3F) | 0x80); // 0x8 = (1000)2, code prefix: (10)2
        byteSink_.writeUInt8((ch & 0x3F) | 0x80);        // 0x8 = (1000)2, code prefix: (10)2
    }
    else if (ch < 0x110000) { // four-byte codes: 11110zzz | 10zzyyyy | 10yyyyxx | 10xxxxxx
        if ((ch & 0xFFFE) == 0xFFFE) throw NonCharacterEncodingError{};
        byteSink_.writeUInt8((ch >> 18) | 0xF0);          // 0xF = (1111)2, code prefix: (11110)2
        byteSink_.writeUInt8(((ch >> 12) & 0x3F) | 0x80); // 0x8 = (1000)2, code prefix: (10)2
        byteSink_.writeUInt8(((ch >> 6) & 0x3F) | 0x80);  // 0x8 = (1000)2, code prefix: (10)2
        byteSink_.writeUInt8((ch & 0x3F) | 0x80);         // 0x8 = (1000)2, code prefix: (10)2
    }
    else {
        throw LargeCodePointEncodingError{};
    }
}

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
