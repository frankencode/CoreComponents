/*
 * BitSink.hpp -- bit-vise writing to a 'Stream'
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_BITSINK_HPP
#define PONA_BITSINK_HPP

#include "atoms"
#include "defaults.hpp"
#include "Stream.hpp"

namespace pona
{

class BitSink: public Instance
{
public:
	BitSink(Ref<Stream> stream, int bufCapacity = PONA_DEFAULT_BUF_CAPA, int endian = PONA_DEFAULT_ENDIAN);
	BitSink(void* buf, int bufCapacity, int endian = PONA_DEFAULT_ENDIAN);
	~BitSink();
	
	void flush();
	
	void writeBit(uint8_t x);
	void writeUInt8(uint8_t x);
	void writeUInt16(uint16_t x);
	void writeUInt32(uint32_t x);
	void writeUInt64(uint64_t x);
	
	void writeUIntVlc(unsigned x, int chunkSize = PONA_VLC_CHUNKSIZE);
	void writeIntVlc(int x, int chunkSize = PONA_VLC_CHUNKSIZE);

	static int bitsPerUIntVlc(unsigned x, int chunkSize = PONA_VLC_CHUNKSIZE);
	static int bitsPerIntVlc(int x, int chunkSize = PONA_VLC_CHUNKSIZE);
	
	void writeUInt(int bits, unsigned x);
	
	uint64_t numBytesWritten() const;
	
private:
	Ref<Stream, Owner> stream_;
	int endian_;

	int bufCapacity_;
	uint8_t* buf_;
	
	int i_;    // byte offset within buf_
	int iBit_;    // bit offset within buf_[i_]
	uint64_t nw_;    // accumulated number of written bytes
};

inline void BitSink::writeBit(uint8_t x)
{
	if (iBit_ == 8)
	{
		iBit_ = 0;
		++i_;
		if (i_ == bufCapacity_) flush();
	}
	buf_[i_] |= (x << iBit_);
	++iBit_;
}

inline void BitSink::writeUInt8(uint8_t x)
{
	for (int i = 0; i < 8; ++i, x >>= 1)
		writeBit(x & 1);
}

inline void BitSink::writeUInt16(uint16_t x)
{
	if (endian_ == LittleEndian)
	{
		writeUInt8((uint8_t)x);
		writeUInt8((uint8_t)(x >> 8));
	}
	else
	{
		writeUInt8((uint8_t)(x >> 8));
		writeUInt8((uint8_t)x);
	}
}

inline void BitSink::writeUInt32(uint32_t x)
{
	if (endian_ == LittleEndian)
	{
		writeUInt8((uint8_t)x);
		writeUInt8((uint8_t)(x >> 8));
		writeUInt8((uint8_t)(x >> 16));
		writeUInt8((uint8_t)(x >> 24));
	}
	else
	{
		writeUInt8((uint8_t)(x >> 24));
		writeUInt8((uint8_t)(x >> 16));
		writeUInt8((uint8_t)(x >> 8));
		writeUInt8((uint8_t)x);
	}
}

inline void BitSink::writeUInt64(uint64_t x)
{
	if (endian_ == LittleEndian)
	{
		for (int i = 0; i <= 7*8; i += 8)
			writeUInt8((uint8_t)(x >> i));
	}
	else
	{
		for (int i = 7*8; i >= 0; i -= 8)
			writeUInt8((uint8_t)(x >> i));
	}
}

inline void BitSink::writeUIntVlc(unsigned x, int chunkSize)
{
	bool hasNext = true;
	while (hasNext)
	{
		hasNext = (x > 0xF);
		writeBit(hasNext);
		for (int i = 0; i < chunkSize; ++i)
		{
			writeBit(x & 1);
			x >>= 1;
		}
	}
}

inline int BitSink::bitsPerUIntVlc(unsigned x, int chunkSize)
{
	bool hasNext = true;
	int n = 0;
	while (hasNext)
	{
		hasNext = (x > 0xF);
		x >>= chunkSize;
		n += chunkSize + 1;
	}
	return n;
}

inline void BitSink::writeIntVlc(int x, int chunkSize)
{
	writeBit(x < 0);
	if (x < 0) x = -x;
	writeUIntVlc(x, chunkSize);
}

inline int BitSink::bitsPerIntVlc(int x, int chunkSize)
{
	if (x < 0) x = -x;
	return bitsPerUIntVlc(x) + 1;
}

inline void BitSink::writeUInt(int bits, unsigned x)
{
	for (int i = 0; i < bits; ++i, x >>= 1)
		writeBit(x & 1);
}

inline uint64_t BitSink::numBytesWritten() const
{
	return nw_ + i_ + (i_ + iBit_ > 0);
}

} // namespace pona

#endif // PONA_BITSINK_HPP
