/*
 * ByteSource.hpp -- byte-vise reading from a 'Stream'
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_BYTESOURCE_HPP
#define PONA_BYTESOURCE_HPP

#include "atom"
#include "defaults.hpp"
#include "Stream.hpp"

namespace pona
{

class ByteSource: public Instance
{
public:
	ByteSource(Ref<Stream> stream, int bufCapa = PONA_DEFAULT_BUF_CAPA, int endian = PONA_DEFAULT_ENDIAN);
	ByteSource(void* buf, int bufCapa, int endian = PONA_DEFAULT_ENDIAN);
	~ByteSource();
	
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
	
	uint64_t numBytesRead() const;
	
	Ref<Stream> stream() const;
	
private:
	Ref<Stream, Owner> stream_;
	int endian_;
	
	void fill();
	
	int bufCapa_;
	int bufFill_;
	uint8_t* buf_;
	int i_;    // byte offset within buf_
	uint64_t nr_;    // accumulated number of bytes read
};

inline uint8_t ByteSource::readUInt8()
{
	if (i_ == bufFill_) fill();
	++nr_;
	return buf_[i_++];
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

inline int8_t ByteSource::readInt8() { return union_cast<int8_t>(readUInt8()); }
inline int16_t ByteSource::readInt16() { return union_cast<int16_t>(readUInt16()); }
inline int32_t ByteSource::readInt32() { return union_cast<int32_t>(readUInt32()); }
inline int64_t ByteSource::readInt64() { return union_cast<int64_t>(readUInt64()); }
inline float32_t ByteSource::readFloat32() { return union_cast<float32_t>(readUInt32()); }
inline float64_t ByteSource::readFloat64() { return union_cast<float64_t>(readUInt64()); }

inline uint64_t ByteSource::numBytesRead() const { return nr_; }

inline Ref<Stream> ByteSource::stream() const { return stream_; }

} // namespace pona

#endif // PONA_BYTESOURCE_HPP
