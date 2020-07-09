/*
 * Copyright (C) 2007-2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Utf16Source>

namespace cc {

bool Utf16Source::Instance::read(uchar_t *ch)
{
    bool more = byteSource_->hasMore();
    if (more) {
        uint16_t w = byteSource_->readUInt16();
        if (w == 0xFFFE && firstTime_) {
            firstTime_ = false;
            byteSource_->setEndian((byteSource_->endian() == ByteOrder::BigEndian) ? ByteOrder::LittleEndian : ByteOrder::BigEndian);
            w = byteSource_->readUInt16();
        }
        *ch = w;
        if ((0xD800 <= *ch) && (*ch < 0xDC00)) {
            uint16_t w = byteSource_->readUInt16();
            if (!((0xDC00 <= w) && (w < 0xE000))) throw DecodingError{};
            *ch = 0x10000 + (((*ch - 0xD800) << 10) | (w - 0xDC00));
        }
        else if (0x10FFFF < *ch) throw DecodingError{};
    }
    return more;
}

String Utf16Source::Instance::DecodingError::message() const
{
    return "UTF-16 error: failed to decode input bytes";
}

} // namespace cc
