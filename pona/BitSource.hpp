/****************************************************************************
**
** This file is part of libPONA - The Portable Network Abstraction Library.
**
** Copyright (C) 2007-2009  Frank Mertens
**
** This file is part of a free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License as published
** by the Free Software Foundation, either version 3 of the License,
** or (at your option) any later version.
**
** The library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this libary.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#ifndef PONA_BITSOURCE_HPP
#define PONA_BITSOURCE_HPP

#include "atoms"
#include "defaults.hpp"
#include "Stream.hpp"

namespace pona
{

class BitSource: public Instance
{
public:
	BitSource(Ref<Stream> stream, int bufCapa = PONA_DEFAULT_BUF_CAPA, int endian = PONA_DEFAULT_ENDIAN);
	BitSource(uint8_t* buf, int bufCapa, int endian = PONA_DEFAULT_ENDIAN);

	~BitSource();
	
	uint8_t readBit();
	uint8_t readUInt8();
	uint16_t readUInt16();
	uint32_t readUInt32();
	uint64_t readUInt64();
	
	unsigned readUIntVlc(int chunkSize = PONA_VLC_CHUNKSIZE);
	int readIntVlc(int chunkSize = PONA_VLC_CHUNKSIZE);
	
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

inline uint8_t BitSource::readBit()
{
	if (iBit_ == 8)
	{
		iBit_ = 0;
		++i_;
		if (i_ >= bufFill_) fill();
	}
	return (buf_[i_] >> (iBit_++)) & 1;
}

inline uint8_t BitSource::readUInt8()
{
	uint8_t h = 0;
	for (int i = 0; i < 8; ++i)
		h |= readBit() << i;
	return h;
}

inline uint16_t BitSource::readUInt16()
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

inline uint32_t BitSource::readUInt32()
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

inline uint64_t BitSource::readUInt64()
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

inline unsigned BitSource::readUIntVlc(int chunkSize)
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

inline int BitSource::readIntVlc(int chunkSize)
{
	int x;
	if (readBit())
		x = -int(readUInt(chunkSize));
	else
		x = readUInt(chunkSize);
	return x;
}

inline unsigned BitSource::readUInt(int bits)
{
	unsigned x = 0;
	for (int i = 0; i < bits; ++i)
		x |= (readBit() << i);
	return x;
}

inline uint64_t BitSource::numBytesRead() const
{
	return nr_ - (/* unconsumed buffer bytes */ bufFill_ - i_ - (i_ + iBit_ > 0));
}

inline Ref<Stream> BitSource::stream() const
{
	return stream_;
}

} // namespace pona

#endif // PONA_BITSOURCE_HPP
