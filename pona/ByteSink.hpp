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

#ifndef PONA_BYTESINK_HPP
#define PONA_BYTESINK_HPP

#include "Atoms.hpp"
#include "Options.hpp"
#include "Stream.hpp"

namespace pona
{

class ByteSink: public Instance
{
public:
	ByteSink();
	ByteSink(Ref<Stream> stream, int bufCapa = PONA_DEFAULT_BUF_CAPA, int endian = PONA_DEFAULT_ENDIAN);
	ByteSink(uint8_t* buf, int bufCapa, int endian = PONA_DEFAULT_ENDIAN);
	~ByteSink();
	
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
	
	int64_t numBytesWritten() const;
	void flush();
	
	Ref<Stream> stream() const;
	
private:
	Ref<Stream, Owner> stream_;
	int endian_;
	
	int bufCapa_;
	uint8_t* buf_;
	int i_;    // byte offset within buf_
	int64_t nw_;    // accumulated number of written bytes
};

inline void ByteSink::writeUInt8(uint8_t x)
{
	if (i_ == bufCapa_) flush();
	if (buf_) buf_[i_] = x;
	++i_;
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

inline void ByteSink::writeInt8(int8_t x) { writeUInt8(union_cast<uint8_t>(x)); }
inline void ByteSink::writeInt16(int16_t x) { writeUInt16(union_cast<uint16_t>(x)); }
inline void ByteSink::writeInt32(int32_t x) { writeUInt32(union_cast<uint32_t>(x)); }
inline void ByteSink::writeInt64(int64_t x) { writeUInt64(union_cast<uint64_t>(x)); }

inline void ByteSink::writeFloat32(float32_t x) { writeUInt32(union_cast<uint32_t>(x)); }
inline void ByteSink::writeFloat64(float64_t x) { writeUInt64(union_cast<uint64_t>(x)); }

inline int64_t ByteSink::numBytesWritten() const
{
	return nw_ + /* consumed buffer bytes */ i_;
}

inline Ref<Stream> ByteSink::stream() const { return stream_; }

} // namespace pona

#endif // PONA_BYTESINK_HPP
