/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Utf16Source>

namespace cc {

bool Utf16Source::read(Out<char32_t> ch)
{
    bool hasMore = byteSource_.prefetch();

    if (hasMore) {
        uint16_t w = byteSource_.readUInt16();
        if (w == 0xFFFE && firstTime_) {
            firstTime_ = false;
            byteSource_.setEndian((byteSource_.endian() == ByteOrder::BigEndian) ? ByteOrder::LittleEndian : ByteOrder::BigEndian);
            w = byteSource_.readUInt16();
        }
        char32_t u = w;
        if ((0xD800 <= u) && (u < 0xDC00)) {
            uint16_t w = byteSource_.readUInt16();
            if (!((0xDC00 <= w) && (w < 0xE000))) throw DecodingError{};
            u = 0x10000 + (((u - 0xD800) << 10) | (w - 0xDC00));
        }
        else if (0x10FFFF < u) throw DecodingError{};

        ch << u;
    }

    return hasMore;
}

String Utf16Source::DecodingError::message() const
{
    return "UTF-16 error: failed to decode input bytes";
}

} // namespace cc
