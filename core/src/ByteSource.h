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

/** \class ByteSource ByteSource.h cc/ByteSource
  * \ingroup binary_io
  * \brief Byte decoding sink
  * \see ByteSink, ReplaySource
  */
class ByteSource: public Object
{
public:
    /** Open a new ByteSource
      * \param source data source to write bytes from
      * \param buffer input buffer to use
      * \param endian endianess for reading multi-byte words
      * \return new object instance
      */
    inline static Ref<ByteSource> open(Stream *source, ByteArray *buffer = 0, Endian endian = LittleEndian) {
        return new ByteSource(source, buffer, endian);
    }

    /** Open a new ByteSource
      * \param buffer input buffer to read bytes from
      * \param endian endianess for reading multi-byte words
      * \return new object instance
      */
    inline static Ref<ByteSource> open(const ByteArray *buffer, Endian endian = LittleEndian) {
        return new ByteSource(0, const_cast<ByteArray *>(buffer), endian);
    }

    /** Read a single byte
      * \param x target to write the byte into
      * \return true if not end of input
      */
    bool readByte(uint8_t *x);

    /** Read a single character
      * \param ch target to write the character into
      * \return true if not end of input
      */
    inline bool readChar(char *ch) { return readByte(reinterpret_cast<uint8_t*>(ch)); }

    /** Read a fixed number of bytes (N)
      * \param span target buffer (N = span->count())
      * \see ByteArray::select()
      */
    void read(ByteArray *span);

    /** Read a fixed number of bytes
      * \param n number of bytes to read
      * \return bytes read
      */
    Ref<ByteArray> readSpan(int n);

    /// Return true if more bytes can be read
    bool hasMore();

    /// Read a byte
    uint8_t readUInt8();

    /// Read a 2-byte word
    uint16_t readUInt16();

    /// Read a 4-byte word
    uint32_t readUInt32();

    /// Read an 8-byte word
    uint64_t readUInt64();

    /// Read a signed byte
    inline int8_t readInt8() { return union_cast<int8_t>(readUInt8()); }

    /// Read a signed 2-byte word
    inline int16_t readInt16() { return union_cast<int16_t>(readUInt16()); }

    /// Read a signed 4-byte word
    inline int32_t readInt32() { return union_cast<int32_t>(readUInt32()); }

    /// Read a signed 8-byte word
    inline int64_t readInt64() { return union_cast<int64_t>(readUInt64()); }

    /// Read a single precision floating point number
    inline float32_t readFloat32() { return union_cast<float32_t>(readUInt32()); }

    /// Read a double precision floating point number
    inline float64_t readFloat64() { return union_cast<float64_t>(readUInt64()); }

    /// Data source to read bytes from
    inline Stream *source() const { return source_; }

    /// The output buffer
    inline ByteArray *buffer() const { return buffer_; }

    /// Current endianess for decoding multi-byte words
    inline Endian endian() const { return endian_; }

    /// Switch endianess
    inline void setEndian(Endian endian) { endian_ = endian; }

    /// Total number of bytes read
    inline off_t currentOffset() const { return i0_ + i_; }

private:
    ByteSource(Stream *source, ByteArray *buffer, Endian endian):
        source_(source),
        buffer_(buffer),
        endian_(endian),
        i0_(0), i_(0),
        n_(source_ ? 0 : buffer->count())
    {
        if (!buffer_) buffer_ = ByteArray::allocate(0x1000);
    }

    Ref<Stream> source_;
    Ref<ByteArray> buffer_;
    Endian endian_;
    off_t i0_;
    int i_, n_;
};

inline bool ByteSource::readByte(uint8_t *x)
{
    bool more = hasMore();
    if (more)
        *x = readUInt8();
    return more;
}

inline void ByteSource::read(ByteArray *span)
{
    for (int i = 0, n = span->count(); i < n; ++i)
        span->at(i) = readUInt8();
}

inline Ref<ByteArray> ByteSource::readSpan(int n)
{
    Ref<ByteArray> bytes = ByteArray::create(n);
    read(bytes);
    return bytes;
}

inline bool ByteSource::hasMore()
{
    if (i_ == n_ && source_) {
        n_ = source_->read(buffer_);
        i0_ += i_;
        i_ = 0;
    }
    return i_ < n_;
}

inline uint8_t ByteSource::readUInt8()
{
    if (i_ == n_) {
        if (!source_) throw UnexpectedEndOfInputError();

        n_ = source_->read(buffer_);
        i_ = 0;

        if (n_ == 0) throw UnexpectedEndOfInputError();
    }

    return buffer_->at(i_++);
}

inline uint16_t ByteSource::readUInt16()
{
    uint16_t h;
    if (endian_ == LittleEndian)
    {
        h  = uint16_t(readUInt8());
        h |= uint16_t(readUInt8()) << 8;
    }
    else // if (endian_ == BigEndian)
    {
        h  = uint16_t(readUInt8()) << 8;
        h |= uint16_t(readUInt8());
    }
    return h;
}

inline uint32_t ByteSource::readUInt32()
{
    uint32_t h;
    if (endian_ == LittleEndian)
    {
        h  = uint32_t(readUInt8());
        h |= uint32_t(readUInt8()) << 8;
        h |= uint32_t(readUInt8()) << 16;
        h |= uint32_t(readUInt8()) << 24;
    }
    else // if (endian_ == BigEndian)
    {
        h  = uint32_t(readUInt8()) << 24;
        h |= uint32_t(readUInt8()) << 16;
        h |= uint32_t(readUInt8()) << 8;
        h |= uint32_t(readUInt8());
    }
    return h;
}

inline uint64_t ByteSource::readUInt64()
{
    uint64_t h = 0;
    if (endian_ == LittleEndian)
    {
        for (int i = 0; i <= 7*8; i += 8)
            h |= uint64_t(readUInt8()) << i;
    }
    else // if (endian_ == BigEndian)
    {
        for (int i = 7*8; i >= 0; i -= 8)
            h |= uint64_t(readUInt8()) << i;
    }
    return h;
}

} // namespace cc
