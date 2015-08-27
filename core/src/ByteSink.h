/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUX_BYTESINK_H
#define FLUX_BYTESINK_H

#include <flux/exceptions>
#include <flux/Stream>

namespace flux {

/** \brief Byte encoding sink
  * \see ByteSource
  */
class ByteSink: public Object
{
public:
    inline static Ref<ByteSink> open(Stream *stream, ByteArray *buf, int endian = LittleEndian) {
        return new ByteSink(stream, buf, endian);
    }
    inline static Ref<ByteSink> open(ByteArray *buf, int endian = LittleEndian) {
        return new ByteSink(0, buf, endian);
    }
    ~ByteSink()
    {
        if (stream_ &&  i_ > 0) flush();
    }

    inline void write(uint8_t x) { writeUInt8(x); }
    inline void write(char ch) { writeUInt8((uint8_t)ch); }

    void write(ByteArray *bytes);

    void writeUInt8(uint8_t x);
    void writeUInt16(uint16_t x);
    void writeUInt32(uint32_t x);
    void writeUInt64(uint64_t x);

    inline void writeInt8(int8_t x) { writeUInt8(union_cast<uint8_t>(x)); }
    inline void writeInt16(int16_t x) { writeUInt16(union_cast<uint16_t>(x)); }
    inline void writeInt32(int32_t x) { writeUInt32(union_cast<uint32_t>(x)); }
    inline void writeInt64(int64_t x) { writeUInt64(union_cast<uint64_t>(x)); }

    inline void writeFloat32(float32_t x) { writeUInt32(union_cast<uint32_t>(x)); }
    inline void writeFloat64(float64_t x) { writeUInt64(union_cast<uint64_t>(x)); }

    inline int bytesCached() const { return i_; }
    void flush();

    inline Stream *stream() const { return stream_; }
    inline ByteArray *buffer() const { return buf_; }

    inline int endian() const { return endian_; }
    inline void setEndian(int endian) { endian_ = endian; }

private:
    ByteSink(Stream *stream, ByteArray *buf, int endian)
        : stream_(stream),
          buf_(buf),
          endian_(endian),
          i_(0)
    {
        if (!buf_) buf_ = ByteArray::allocate(0x4000);
    }

    Ref<Stream> stream_;
    Ref<ByteArray> buf_;
    int endian_;
    int i_;
};

inline void ByteSink::flush()
{
    if (!stream_) throw BufferOverflow();

    stream_->write(buf_->select(0, i_));
    i_ = 0;
}

inline void ByteSink::write(ByteArray *bytes)
{
    for (int i = 0, n = bytes->count(); i < n; ++i)
        write(bytes->at(i));
}

inline void ByteSink::writeUInt8(uint8_t x)
{
    if (i_ == buf_->count()) flush();
    buf_->at(i_++) = x;
}

inline void ByteSink::writeUInt16(uint16_t x)
{
    if (endian_ == LittleEndian)
    {
        writeUInt8(uint8_t(x));
        writeUInt8(uint8_t(x >> 8));
    }
    else
    {
        writeUInt8(uint8_t(x >> 8));
        writeUInt8(uint8_t(x));
    }
}

inline void ByteSink::writeUInt32(uint32_t x)
{
    if (endian_ == LittleEndian)
    {
        writeUInt8(uint8_t(x));
        writeUInt8(uint8_t(x >> 8));
        writeUInt8(uint8_t(x >> 16));
        writeUInt8(uint8_t(x >> 24));
    }
    else
    {
        writeUInt8(uint8_t(x >> 24));
        writeUInt8(uint8_t(x >> 16));
        writeUInt8(uint8_t(x >> 8));
        writeUInt8(uint8_t(x));
    }
}

inline void ByteSink::writeUInt64(uint64_t x)
{
    if (endian_ == LittleEndian) {
        for (int i = 0; i < int(sizeof(x)); ++i, x >>= 8)
            writeUInt8(uint8_t(x));
    }
    else {
        for (int i = (int(sizeof(x))-1) * 8; i >= 0; i -= 8)
            writeUInt8(uint8_t(x >> i));
    }
}

} // namespace flux

#endif // FLUX_BYTESINK_H
