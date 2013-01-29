 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */
#ifndef FTL_BYTEDECODER_HPP
#define FTL_BYTEDECODER_HPP

#include "atoms"
#include "defaults.hpp"
#include "Stream.hpp"

namespace ftl
{

class ByteDecoder: public Source<uint8_t>, Source<char>
{
public:
	inline static hook<ByteDecoder> open(Stream *stream, int bufCapa = FTL_DEFAULT_BUF_CAPA, int endian = FTL_DEFAULT_ENDIAN) {
		return new ByteDecoder(stream, bufCapa, endian);
	}
	inline static hook<ByteDecoder> open(const void *buf, int bufCapa, int endian = FTL_DEFAULT_ENDIAN) {
		return new ByteDecoder(buf, bufCapa, endian);
	}
	~ByteDecoder();

	bool read(uint8_t *x);
	bool read(char *ch);

	void read(ByteArray *bytes);
	hook<ByteArray> read(int n);

	bool hasMore();
	uint8_t readUInt8();
	uint16_t readUInt16();
	uint32_t readUInt32();
	uint64_t readUInt64();

	int8_t readInt8();
	int16_t readInt16();
	int32_t readInt32();
	int64_t readInt64();

	float32_t readFloat32();
	float64_t readFloat64();

	void skipPad(int padSize);

	off_t numBytesRead() const;

	Stream *stream() const;

	bool endian() const;
	void setEndian(int endian);

private:
	ByteDecoder(Stream *stream, int bufCapa, int endian);
	ByteDecoder(const void *buf, int bufCapa, int endian);

	hook<Stream> stream_;
	int endian_;

	void fill();

	int bufCapa_;
	int bufFill_;
	uint8_t *buf_;
	int i_;    // byte offset within buf_
	off_t nr_;    // accumulated number of bytes read
};

inline bool ByteDecoder::read(uint8_t *x)
{
	bool more = hasMore();
	if (more)
		*x = readUInt8();
	return more;
}

inline bool ByteDecoder::read(char *ch)
{
	return read(reinterpret_cast<uint8_t*>(ch));
}

inline void ByteDecoder::read(ByteArray *bytes)
{
	for (int i = 0, n = bytes->length(); i < n; ++i)
		bytes->set(i, readUInt8());
}

inline hook<ByteArray> ByteDecoder::read(int n)
{
	hook<ByteArray> bytes = ByteArray::create(n);
	read(bytes);
	return bytes;
}

inline bool ByteDecoder::hasMore()
{
	bool more = i_ < bufFill_;
	if (!more) {
		if (stream_) {
			bufFill_ = stream_->readAvail(buf_, bufCapa_);
			i_ = 0;
			more = bufFill_ > 0;
		}
	}
	return more;
}

inline uint8_t ByteDecoder::readUInt8()
{
	if (i_ == bufFill_) fill();
	++nr_;
	return buf_[i_++];
}

inline uint16_t ByteDecoder::readUInt16()
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

inline uint32_t ByteDecoder::readUInt32()
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

inline uint64_t ByteDecoder::readUInt64()
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

inline int8_t ByteDecoder::readInt8() { return union_cast<int8_t>(readUInt8()); }
inline int16_t ByteDecoder::readInt16() { return union_cast<int16_t>(readUInt16()); }
inline int32_t ByteDecoder::readInt32() { return union_cast<int32_t>(readUInt32()); }
inline int64_t ByteDecoder::readInt64() { return union_cast<int64_t>(readUInt64()); }
inline float32_t ByteDecoder::readFloat32() { return union_cast<float32_t>(readUInt32()); }
inline float64_t ByteDecoder::readFloat64() { return union_cast<float64_t>(readUInt64()); }

inline void ByteDecoder::skipPad(int padSize) {
	while ((numBytesRead() & (padSize - 1)) != 0)
		readUInt8();
}

inline off_t ByteDecoder::numBytesRead() const { return nr_; }

inline Stream *ByteDecoder::stream() const { return stream_; }

inline bool ByteDecoder::endian() const { return endian_; }
inline void ByteDecoder::setEndian(int endian) { endian_ = endian; }

} // namespace ftl

#endif // FTL
