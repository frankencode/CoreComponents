/*
 * Crc32.hpp -- CRC32 as described in ISO 3309 or ITU-T V.42
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */
#ifndef FTL_CRC32_HPP
#define FTL_CRC32_HPP

#include "types.hpp"

namespace ftl
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

} // namespace ftl

#endif // FTL_CRC32_HPP
