/*
 * Copyright (C) 2025 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/Base64>
#include <cc/BitSource>
#include <cc/BitSink>

namespace cc {

String Base64::encode(const Bytes &data, Alphabet alphabet, Padding padding)
{
    if (padding == Padding::Auto) {
        if (alphabet == Alphabet::Mime) {
            padding = Padding::Mime;
        } else {
            padding = Padding::None;
        }
    }

    String buffer = String::allocate(encodedSize(data, padding));

    BitSource bitSource { data };
    long i = 0;

    for (unsigned j = 0; bitSource.read<6, unsigned>(&j);) {
        char ch = '\0';
        if (j < 26) {
            ch = 'A' + j;
        }
        else if (j < 52) {
            ch = 'a' + (j - 26);
        }
        else if (j < 62) {
            ch = '0' + (j - 52);
        }
        else if (alphabet == Alphabet::Mime) {
            ch = (j == 62) ? '+' : '/';
        }
        else if (alphabet == Alphabet::UrlSafe) {
            ch = (j == 62) ? '-' : '_';
        }

        buffer[i++] = ch;
    }

    while (i < buffer.count()) {
        buffer[i++] = '=';
    }

    return buffer;
}

String Base64::decode(const Bytes &text)
{
    String buffer = String::allocate(decodedSize(text));

    {
        BitSink bitSink { buffer };
        for (uint8_t ch: text) {
            if ('A' <= ch && ch <= 'Z') ch -= static_cast<uint8_t>('A');
            else if ('a' <= ch && ch <= 'z') ch = ch - static_cast<uint8_t>('a') + 26;
            else if ('0' <= ch && ch <= '9') ch = ch - static_cast<uint8_t>('0') + 52;
            else if (ch == '+' || ch == '-') ch = 62;
            else if (ch == '/' || ch == '_') ch = 63;
            else if (ch == '=') break;
            bitSink.writeBits<6>(ch);
        }
    }

    return buffer;
}

long Base64::encodedSize(const Bytes &data, Padding padding)
{
    long m = 0;
    if (padding == Padding::Mime) {
        const long n = data.count();
        m = 4 * ((n + 2) / 3);
    }
    else {
        const long n = 8 * data.count();
        m = (4 * n + 2) / 3;
    }
    return m;
}

long Base64::decodedSize(const Bytes &text)
{
    long n = text.count();
    while (n > 0 && text.at(n - 1) == '=') --n;
    return (n * 6) / 8;
}

} // namespace cc
