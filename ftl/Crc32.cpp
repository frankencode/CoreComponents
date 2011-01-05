/*
 * Crc32.cpp -- CRC32 as described in ISO 3309 or ITU-T V.42
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */

#include "strings.hpp"
#include "Crc32.hpp"

namespace ftl
{

Crc32::Crc32(uint32_t seed)
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

void Crc32::reset(uint32_t seed)
{
	crc_ = seed;
}

void Crc32::feed(const void* buf, int bufSize)
{
	uint32_t crc = crc_;
	for (int i = 0; i < bufSize; ++i)
		crc = crcTable_[(crc ^ reinterpret_cast<const uint8_t*>(buf)[i]) & 0xFF] ^ (crc >> 8);
	crc_ = crc;
}

} // namespace ftl
