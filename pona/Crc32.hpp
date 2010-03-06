/*
 * Crc32.hpp -- CRC32 as described in ISO 3309 or ITU-T V.42
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_CRC32_HPP
#define PONA_CRC32_HPP

#include "types.hpp"

namespace pona
{

class Crc32
{
public:
	Crc32(uint32_t seed = ~0);
	void reset(uint32_t seed = ~0);
	
	void feed(const void* buf, int bufSize);
	inline uint32_t sum() const { return crc_; }
	
private:
   uint32_t crcTable_[256];
   uint32_t crc_;
};

inline uint32_t crc32(const void* buf, int bufSize) {
	Crc32 crc;
	crc.feed(buf, bufSize);
	return crc.sum();
}

} // namespace pona

#endif // PONA_CRC32_HPP
