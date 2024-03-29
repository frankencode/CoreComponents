/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/Utf8Source>

namespace cc {

String Utf8Source::DecodingError::message() const
{
    return "UTF-8 error: failed to decode input bytes";
}

char32_t Utf8Source::readMultiByte(char32_t ch)
{
    int n = -1; // number of additional bytes

    // translate prefix to code length (n is the number of successive bytes)
    if ((ch >> 5) == 6) { // code prefix: 6 = (110)2
        ch = ch & 0x1F;
        n = 1;
    }
    else if ((ch >> 4) == 14) { // code prefix: 14 = (1110)2
        ch = ch & 0x0F;
        n = 2;
    }
    else if ((ch >> 3) == 30) { // code prefix: 30 = (11110)2
        ch = ch & 0x07;
        n = 3;
    }

    int n2 = n;

    // read n successive characters (chs), which carry the code prefix (10)2
    while (n > 0) {
        uint8_t chs = byteSource_.readUInt8();
        if ((chs >> 6) == 2) // 2 = (10)2
            ch = (ch << 6) | (chs & 0x3F);
        else
            break;
        --n;
    }

    // enforce minimum code length
    if (n == 0) {
        if (n2 == 1) {
            if (ch < 0x80) n = -1;
        }
        else if (n2 == 2) {
            if (ch < 0x800) n = -1;
        }
        else if (n2 == 3) {
            if (ch < 0x10000) n = -1;
        }
    }

    if (n < 0) throw DecodingError{};

    return ch;
}

} // namespace cc
