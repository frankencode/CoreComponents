/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUX_UTF16SOURCE_H
#define FLUX_UTF16SOURCE_H

#include <flux/ByteSource>
#include <flux/utf16>

namespace flux {

/** \brief Unicode UTF-16 decoding data source
  *
  * A Byte-Order-Mark (BOM) at the start of the stream automatically sets
  * endianess. The decoder is designed to be as strict as possible. It checks
  * if the input streams delivers full 16-bit words and it requires high
  * surrogates to be followed by low surrogates.
  */
class Utf16Source: public Object
{
public:
    inline static Ref<Utf16Source> open(Stream *stream, ByteArray *buf, int endian = BigEndian) {
        return new Utf16Source(stream, buf, endian);
    }
    inline static Ref<Utf16Source> open(ByteArray *buf, int endian = BigEndian) {
        return new Utf16Source(0, buf, endian);
    }

    inline bool read(uchar_t *ch)
    {
        bool more = byteSource_->hasMore();
        if (more) {
            uint16_t w = byteSource_->readUInt16();
            if (w == 0xFFFE && firstTime_) {
                firstTime_ = false;
                byteSource_->setEndian((byteSource_->endian() == BigEndian) ? LittleEndian : BigEndian);
                w = byteSource_->readUInt16();
            }
            *ch = w;
            if ((0xD800 <= *ch) && (*ch < 0xDC00)) {
                uint16_t w = byteSource_->readUInt16();
                if (!((0xDC00 <= w) && (w < 0xE000))) throw utf16::DecodeError();
                *ch = 0x10000 + (((*ch - 0xD800) << 10) | (w - 0xDC00));
            }
            else if (0x10FFFF < *ch) throw utf16::DecodeError();
        }
        return more;
    }

    inline ByteSource *byteSource() const { return byteSource_; }

private:
    Utf16Source(Stream *stream, ByteArray *buf, int endian)
        : byteSource_(ByteSource::open(stream, buf, endian)),
          firstTime_(true)
    {}

    Ref<ByteSource> byteSource_;
    bool firstTime_;
};

} // namespace flux

#endif // FLUX_UTF16SOURCE_H
