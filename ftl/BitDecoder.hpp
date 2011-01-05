/*
 * BitDecoder.hpp -- bit-vise reading from a 'Stream'
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */
#ifndef FTL_BITDECODER_HPP
#define FTL_BITDECODER_HPP

#include "atoms"
#include "defaults.hpp"
#include "Stream.hpp"

namespace ftl
{

class BitDecoder: public Instance
{
public:
	BitDecoder(Ref<Stream> stream, int bufCapa = FTL_DEFAULT_BUF_CAPA, int endian = FTL_DEFAULT_ENDIAN);
	BitDecoder(void* buf, int bufCapa, int endian = FTL_DEFAULT_ENDIAN);
	~BitDecoder();
	
	uint8_t readBit();
	uint8_t readUInt8();
	uint16_t readUInt16();
	uint32_t readUInt32();
	uint64_t readUInt64();
	
	unsigned readUIntVlc(int chunkSize = FTL_VLC_CHUNKSIZE);
	int readIntVlc(int chunkSize = FTL_VLC_CHUNKSIZE);
	
	unsigned readUInt(int bits);
	
	uint64_t numBytesRead() const;
	
	Ref<Stream> stream() const;

private:
	void fill();
	
	Ref<Stream, Owner> stream_;
	int endian_;

	int bufCapa_;
	int bufFill_;
	uint8_t* buf_;

	int i_;    // byte offset within buf_
	int iBit_;    // bit offset in buf_[i]
	uint64_t nr_;    // accumulated number of bytes read
};

inline uint8_t BitDecoder::readBit()
{
	if (iBit_ == 8)
	{
		iBit_ = 0;
		++i_;
		if (i_ >= bufFill_) fill();
	}
	return (buf_[i_] >> (iBit_++)) & 1;
}

inline uint8_t BitDecoder::readUInt8()
{
	uint8_t h = 0;
	for (int i = 0; i < 8; ++i)
		h |= readBit() << i;
	return h;
}

inline uint16_t BitDecoder::readUInt16()
{
	uint16_t h;
	
	if (endian_ == LittleEndian)
	{
		h = uint16_t(readUInt8());
		h |= uint16_t(readUInt8()) << 8;
	}
	else
	{
		h = uint16_t(readUInt8()) << 8;
		h |= uint16_t(readUInt8());
	}

	return h;
}

inline uint32_t BitDecoder::readUInt32()
{
	uint32_t h;
	
	if (endian_ == LittleEndian)
	{
		h = uint32_t(readUInt8());
		h |= uint32_t(readUInt8()) << 8;
		h |= uint32_t(readUInt8()) << 16;
		h |= uint32_t(readUInt8()) << 24;
	}
	else
	{
		h = uint32_t(readUInt8()) << 24;
		h |= uint32_t(readUInt8()) << 16;
		h |= uint32_t(readUInt8()) << 8;
		h |= uint32_t(readUInt8());
	}

	return h;
}

inline uint64_t BitDecoder::readUInt64()
{
	uint64_t h = 0;
	
	if (endian_ == LittleEndian)
	{
		for (int i = 0; i <= 7*8; i += 8)
			h |= uint64_t(readUInt8()) << i;
	}
	else
	{
		for (int i = 7*8; i >= 0; i -= 8)
			h |= uint64_t(readUInt8()) << i;
	}
	
	return h;
}

inline unsigned BitDecoder::readUIntVlc(int chunkSize)
{
	unsigned x = 0;
	int i = 0;
	int hasNext = 1;
	while (hasNext)
	{
		hasNext = readBit();
		for (int k = 0; k < chunkSize; ++k)
		{
			x |= unsigned(readBit()) << i;
			++i;
		}
	}
	return x;
}

inline int BitDecoder::readIntVlc(int chunkSize)
{
	int x;
	if (readBit())
		x = -int(readUInt(chunkSize));
	else
		x = readUInt(chunkSize);
	return x;
}

inline unsigned BitDecoder::readUInt(int bits)
{
	unsigned x = 0;
	for (int i = 0; i < bits; ++i)
		x |= (readBit() << i);
	return x;
}

inline uint64_t BitDecoder::numBytesRead() const
{
	return nr_ - (/* unconsumed buffer bytes */ bufFill_ - i_ - (i_ + iBit_ > 0));
}

inline Ref<Stream> BitDecoder::stream() const
{
	return stream_;
}

} // namespace ftl

#endif // FTL_BITDECODER_HPP
