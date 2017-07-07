/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/ByteSource>
#include <cc/utf16>

namespace cc {

/** \class Utf16Source Utf16Source.h cc/Utf16Source
  * \ingroup unicode
  * \brief %Unicode UTF-16 decoding data source
  *
  * A Byte-Order-Mark (BOM) at the start of the stream automatically sets
  * endianess. The decoder is designed to be as strict as possible. It checks
  * if the input streams delivers full 16-bit words and it requires high
  * surrogates to be followed by low surrogates.
  */
class Utf16Source: public Object
{
public:
    /** Open a new UTF-16 decoding data source
      * \param stream underlying data stream
      * \param buffer input buffer (optional)
      * \param endian endianess of the 16 bit words transmitted
      * \return new object instance
      */
    inline static Ref<Utf16Source> open(Stream *stream, ByteArray *buffer = 0, Endian endian = BigEndian) {
        return new Utf16Source(stream, buffer, endian);
    }

    /** Open a new UTF-16 decoding data source
      * \param buffer input buffer
      * \param endian endianess of the 16 bit words transmitted
      * \return new object instance
      */
    inline static Ref<Utf16Source> open(ByteArray *buffer, Endian endian = BigEndian) {
        return new Utf16Source(0, buffer, endian);
    }

    /** Read the next %Unicode character
      * \param ch returns the %Unicode character read if any
      * \return true if not end of information, false otherwise
      */
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

    /// Underlying byte source used for UTF-16 decoding
    inline ByteSource *byteSource() const { return byteSource_; }

private:
    Utf16Source(Stream *stream, ByteArray *buffer, Endian endian):
        byteSource_(ByteSource::open(stream, buffer, endian)),
        firstTime_(true)
    {}

    Ref<ByteSource> byteSource_;
    bool firstTime_;
};

} // namespace cc
