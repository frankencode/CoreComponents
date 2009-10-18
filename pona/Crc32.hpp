/*
 * Crc32.hpp -- CRC32 as described in ISO 3309 or ITU-T V.42
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_CRC32_HPP
#define PONA_CRC32_HPP

#include "types.hpp"

namespace pona
{

class PONA_API Crc32
{
public:
	Crc32(uint32_t seed = ~0);
	void reset(uint32_t seed = ~0);
	
	void feed(void* buf, int bufSize);
	uint32_t sum() const;
 
private:
   uint32_t crcTable_[256];
   uint32_t crc_;
};

} // namespace pona

#endif // PONA_CRC32_HPP
