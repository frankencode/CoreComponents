/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/exceptions>
#include <cc/Stream>

namespace cc {

/** \class ByteSink ByteSink.h cc/ByteSink
  * \ingroup binary_io
  * \brief Byte encoding sink
  * \see ByteSource, CaptureSink
  */
class ByteSink: public Object
{
public:
    /** Open a new ByteSink
      * \param sink datra sink to write bytes to
      * \param buffer output buffer to use
      * \param endian endianess for writing multi-byte words
      * \return new object instance
      */
    inline static Ref<ByteSink> open(Stream *sink, ByteArray *buffer, Endian endian = LittleEndian) {
        return new ByteSink(sink, buffer, endian);
    }

    /** Open a new ByteSink
      * \param sink sink to write bytes to
      * \param endian endianess for writing multi-byte words
      * \return new object instance
      */
    inline static Ref<ByteSink> open(Stream *sink, Endian endian = LittleEndian) {
        return new ByteSink(sink, 0, endian);
    }

    /** Open a new ByteSink
      * \param buffer output buffer to use
      * \param endian endianess for writing multi-byte words
      * \return new object instance
      */
    inline static Ref<ByteSink> open(ByteArray *buffer, Endian endian = LittleEndian) {
        return new ByteSink(0, buffer, endian);
    }

    /// write a single character
    inline void write(char ch) { writeUInt8((uint8_t)ch); }

    /// write a string of bytes
    void write(String s);

    /// Write a single byte
    void writeUInt8(uint8_t x);

    /// Write a 2-byte word
    void writeUInt16(uint16_t x);

    /// Write a 4-byte word
    void writeUInt32(uint32_t x);

    /// Write an 8-byte word
    void writeUInt64(uint64_t x);

    /// Write a signed byte
    inline void writeInt8(int8_t x) { writeUInt8(union_cast<uint8_t>(x)); }

    /// Write a signed 2-byte word
    inline void writeInt16(int16_t x) { writeUInt16(union_cast<uint16_t>(x)); }

    /// Write a signed 4-byte word
    inline void writeInt32(int32_t x) { writeUInt32(union_cast<uint32_t>(x)); }

    /// Write a signed 8-byte word
    inline void writeInt64(int64_t x) { writeUInt64(union_cast<uint64_t>(x)); }

    /// Write a simple precision floating point number
    inline void writeFloat32(float32_t x) { writeUInt32(union_cast<uint32_t>(x)); }

    /// Write a double precision floating point number
    inline void writeFloat64(float64_t x) { writeUInt64(union_cast<uint64_t>(x)); }

    /// Flush the output buffer and write cached bytes to the output sink
    void flush();

    /// The output sink
    inline Stream *sink() const { return sink_; }

    /// The output buffer
    inline ByteArray *buffer() const { return buffer_; }

    /// Current endianess for encoding multi-byte words
    inline Endian endian() const { return endian_; }

    /// Switch endianess
    inline void setEndian(Endian endian) { endian_ = endian; }

    /// Total number of bytes written
    inline off_t currentOffset() const { return i0_ + i_; }

private:
    ByteSink(Stream *sink, ByteArray *buffer, Endian endian):
        sink_(sink),
        buffer_(buffer),
        endian_(endian),
        i0_(0), i_(0)
    {
        if (!buffer_) buffer_ = ByteArray::allocate(0x1000);
    }

    ~ByteSink()
    {
        if (sink_ &&  i_ > 0) flush();
    }

    Ref<Stream> sink_;
    Ref<ByteArray> buffer_;
    Endian endian_;
    off_t i0_;
    int i_;
};

inline void ByteSink::flush()
{
    if (!sink_) throw BufferOverflow();

    sink_->write(buffer_->select(0, i_));
    i0_ += i_;
    i_ = 0;
}

inline void ByteSink::write(String s)
{
    for (int i = 0, n = s->count(); i < n; ++i)
        write(s->at(i));
}

inline void ByteSink::writeUInt8(uint8_t x)
{
    if (i_ == buffer_->count()) flush();
    buffer_->at(i_++) = x;
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

} // namespace cc
