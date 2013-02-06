 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */
#ifndef FTL_BYTEENCODER_HPP
#define FTL_BYTEENCODER_HPP

#include "defaults.hpp"
#include "Stream.hpp"

namespace ftl
{

class ByteEncoder: public Sink<uint8_t>, public Sink<char>
{
public:
	inline static Ref<ByteEncoder> open(Stream *stream, int bufCapa = FTL_DEFAULT_BUF_CAPA, int endian = FTL_DEFAULT_ENDIAN) {
		return new ByteEncoder(stream, bufCapa, endian);
	}
	inline static Ref<ByteEncoder> open(void *buf, int bufCapa, int endian = FTL_DEFAULT_ENDIAN) {
		return new ByteEncoder(buf, bufCapa, endian);
	}
	~ByteEncoder();

	void write(uint8_t x);
	void write(char ch);

	void write(ByteArray *bytes);

	void writeUInt8(uint8_t x);
	void writeUInt16(uint16_t x);
	void writeUInt32(uint32_t x);
	void writeUInt64(uint64_t x);

	void writeInt8(int8_t x);
	void writeInt16(int16_t x);
	void writeInt32(int32_t x);
	void writeInt64(int64_t x);

	void writeFloat32(float32_t x);
	void writeFloat64(float64_t x);

	void writePad(int padSize);

	off_t numBytesWritten() const;
	void flush();

	Stream *stream() const;

	int endian() const;
	void setEndian(int endian);

private:
	ByteEncoder();
	ByteEncoder(Stream *stream, int bufCapa, int endian);
	ByteEncoder(void *buf, int bufCapa, int endian);

	Ref<Stream> stream_;
	int endian_;

	int bufCapa_;
	uint8_t *buf_;
	int i_;    // byte offset within buf_
	off_t nw_;    // accumulated number of written bytes
};

inline void ByteEncoder::write(uint8_t x)
{
	writeUInt8(x);
}

inline void ByteEncoder::write(char ch)
{
	writeUInt8((uint8_t)ch);
}

inline void ByteEncoder::write(ByteArray *bytes)
{
	for (int i = 0, n = bytes->length(); i < n; ++i)
		write(bytes->get(i));
}

inline void ByteEncoder::writeUInt8(uint8_t x)
{
	if (i_ == bufCapa_) flush();
	if (buf_) buf_[i_] = x;
	++i_;
}

inline void ByteEncoder::writeUInt16(uint16_t x)
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

inline void ByteEncoder::writeUInt32(uint32_t x)
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

inline void ByteEncoder::writeUInt64(uint64_t x)
{
	if (endian_ == LittleEndian)
	{
		for (int i = 0; i < int(sizeof(x)); ++i, x >>= 8)
			writeUInt8(uint8_t(x));
	}
	else
	{
		for (int i = (int(sizeof(x))-1) * 8; i >= 0; i -= 8)
			writeUInt8(uint8_t(x >> i));
	}
}

inline void ByteEncoder::writeInt8(int8_t x) { writeUInt8(union_cast<uint8_t>(x)); }
inline void ByteEncoder::writeInt16(int16_t x) { writeUInt16(union_cast<uint16_t>(x)); }
inline void ByteEncoder::writeInt32(int32_t x) { writeUInt32(union_cast<uint32_t>(x)); }
inline void ByteEncoder::writeInt64(int64_t x) { writeUInt64(union_cast<uint64_t>(x)); }

inline void ByteEncoder::writeFloat32(float32_t x) { writeUInt32(union_cast<uint32_t>(x)); }
inline void ByteEncoder::writeFloat64(float64_t x) { writeUInt64(union_cast<uint64_t>(x)); }

inline void ByteEncoder::writePad(int padSize) {
	while ((numBytesWritten() & (padSize - 1)) != 0)
		writeUInt8(0);
}

inline off_t ByteEncoder::numBytesWritten() const
{
	return nw_ + /* consumed buffer bytes */ i_;
}

inline Stream *ByteEncoder::stream() const { return stream_; }

inline int ByteEncoder::endian() const { return endian_; }
inline void ByteEncoder::setEndian(int endian) { endian_ = endian; }

} // namespace ftl

#endif // FTL_BYTEENCODER_HPP
