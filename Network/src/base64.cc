/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/base64>
#include <cc/ByteSink>

namespace cc {

String base64Encode(const Bytes &source)
{
    const char *alphabet =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";

    const long n = source.count();
    long i = 0;

    const long m = 4 * (n / 3 + (n % 3 != 0));
    String sink = String::allocate(m);
    long l = 0;

    while (i < n) {
        uint32_t bits = 0;
        for (int j = 0; j < 3; ++j) {
            bits |= (i < n) ? uint32_t(source.byteAt(i)) : uint32_t(0);
            bits <<= 8;
            ++i;
        }
        bits >>= 8;

        for (int k = 0; k < 4; ++k) {
            sink.at(l++) = alphabet[(bits & 0xfc0000) >> 18];
            bits <<= 6;
        }
    }
    if (i > 0) {
        while (i > n) {
            sink.at(--l) = '=';
            --i;
        }
    }

    return sink;
}

String base64Decode(const String &source)
{
    if (source.count() % 4 != 0) throw Base64IllegalInputSizeError{};

    const long m = source.count();
    long p = 0;
    while (m - p > 0) {
        ++p;
        if (source.at(m - p) != '=') {
            --p;
            break;
        }
    }

    if (!(0 <= p && p <= 2)) throw Base64IllegalPaddingError{};

    long n = 3 * (m / 4) - p;
    String sink = String::allocate(n);
    long i = 0;

    for (long l = 0; l < m;) {
        uint32_t bits = 0;
        for (int k = 0; k < 4; ++k) {
            uint32_t ch = source.at(l++);
            if ('A' <= ch && ch <= 'Z') ch -= 'A';
            else if ('a' <= ch && ch <= 'z') ch = (ch - 'a') + 26;
            else if ('0' <= ch && ch <= '9') ch = (ch - '0') + 52;
            else if (ch == '+') ch = 62;
            else if (ch == '/') ch = 63;
            else if (ch == '=') ch = 0;
            else throw Base64IllegalCharacterError{};
            bits |= ch;
            bits <<= 6;
        }
        bits >>= 6;
        for (int j = 0; j < 3; ++j) {
            if (i == n) break;
            uint8_t ch = (bits & 0xFF0000u) >> 16;
            bits <<= 8;
            sink.byteAt(i++) = ch;
        }
        if (i == n) break;
    }

    return sink;
}

} // namespace cc
