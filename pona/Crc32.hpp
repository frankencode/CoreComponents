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

#ifndef PONA_CRC32_HPP
#define PONA_CRC32_HPP

#include "Types.hpp"

namespace pona
{

/** CRC32 as described in ISO 3309 or ITU-T V.42
  */
class Crc32
{
public:
	Crc32(uint32_t seed = ~0)
		: crc_(seed)
	{
		for (int i = 0; i < 256; ++i)
		{
			uint32_t c = i;
			for (int k = 0; k < 8; ++k)
			{
				if (c & 1)
					c = 0xedb88320L ^ (c >> 1);
				else
					c >>= 1;
			}
			crcTable_[i] = c;
		}
	}
	
	void reset(uint32_t seed = ~0)
	{
		crc_ = seed;
	}
	
	void update(uint8_t *buf, int bufSize)
	{
		uint32_t crc = crc_;
		for (int i = 0; i < bufSize; ++i)
			crc = crcTable_[(crc ^ buf[i]) & 0xFF] ^ (crc >> 8);
		crc_ = crc;
	}
	
	uint32_t sum() const { return crc_; }
 
private:
   uint32_t crcTable_[256];
   uint32_t crc_;
};

} // namespace pona

#endif // PONA_CRC32_HPP
