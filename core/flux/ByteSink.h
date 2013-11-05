/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_BYTESINK_H
#define FLUX_BYTESINK_H

#include "Stream.h"

namespace flux
{

class ByteSink: public Sink<uint8_t>, public Sink<char>
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
		if (!buf_)
			buf_ = ByteArray::create(0x3FFF);
	}

	Ref<Stream> stream_;
	Ref<ByteArray> buf_;
	int endian_;
	int i_;
};

inline void ByteSink::flush()
{
	if (!stream_)
		FLUX_THROW(EncodingException, "Output buffer exhausted");
	stream_->write(ByteRange(buf_, 0, i_));
	i_ = 0;
}

inline void ByteSink::write(ByteArray *bytes)
{
	for (int i = 0, n = bytes->size(); i < n; ++i)
		write(bytes->at(i));
}

inline void ByteSink::writeUInt8(uint8_t x)
{
	if (i_ == buf_->size()) flush();
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
