/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FKIT_BYTESOURCE_H
#define FKIT_BYTESOURCE_H

#include "Stream.h"

namespace fkit
{

class ByteSource: public Source<uint8_t>, Source<char>
{
public:
	inline static Ref<ByteSource> open(Stream *stream, ByteArray *buf = 0, int endian = LittleEndian) {
		return new ByteSource(stream, buf, endian);
	}
	inline static Ref<ByteSource> open(ByteArray *buf, int endian = LittleEndian) {
		return new ByteSource(0, buf, endian);
	}

	bool read(uint8_t *x);
	inline bool read(char *ch) { return read(reinterpret_cast<uint8_t*>(ch)); }

	void read(ByteArray *bytes);
	Ref<ByteArray> read(int n);

	bool hasMore();

	uint8_t readUInt8();
	uint16_t readUInt16();
	uint32_t readUInt32();
	uint64_t readUInt64();

	inline int8_t readInt8() { return union_cast<int8_t>(readUInt8()); }
	inline int16_t readInt16() { return union_cast<int16_t>(readUInt16()); }
	inline int32_t readInt32() { return union_cast<int32_t>(readUInt32()); }
	inline int64_t readInt64() { return union_cast<int64_t>(readUInt64()); }
	inline float32_t readFloat32() { return union_cast<float32_t>(readUInt32()); }
	inline float64_t readFloat64() { return union_cast<float64_t>(readUInt64()); }

	inline Stream *stream() const { return stream_; }
	inline ByteArray *buffer() const { return buf_; }

	inline bool endian() const { return endian_; }
	inline void setEndian(int endian) { endian_ = endian; }

private:
	ByteSource(Stream *stream, ByteArray *buf, int endian)
		: stream_(stream),
		  buf_(buf),
		  endian_(endian),
		  i_(0),
		  n_(stream_ ? 0 : buf->size())
	{
		if (!buf_)
			buf_ = ByteArray::create(0x3FFF);
	}

	Ref<Stream> stream_;
	Ref<ByteArray> buf_;
	int endian_;
	int i_, n_;
};

inline bool ByteSource::read(uint8_t *x)
{
	bool more = hasMore();
	if (more)
		*x = readUInt8();
	return more;
}

inline void ByteSource::read(ByteArray *bytes)
{
	for (int i = 0, n = bytes->size(); i < n; ++i)
		bytes->at(i) = readUInt8();
}

inline Ref<ByteArray> ByteSource::read(int n)
{
	Ref<ByteArray> bytes = ByteArray::create(n);
	read(bytes);
	return bytes;
}

inline bool ByteSource::hasMore()
{
	if (i_ == n_ && stream_) {
		n_ = stream_->readAvail(buf_);
		i_ = 0;
	}
	return i_ < n_;
}

inline uint8_t ByteSource::readUInt8()
{
	if (i_ == n_) {
		if (!stream_)
			FKIT_THROW(EncodingException, "Unexpected end of input");

		n_ = stream_->readAvail(buf_);
		i_ = 0;

		if (n_ == 0)
			FKIT_THROW(EncodingException, "Unexpected end of input");
	}

	return buf_->at(i_++);
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

} // namespace fkit

#endif // FKIT_BYTESOURCE_H
