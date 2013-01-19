 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */
#ifndef FTL_CRC32_HPP
#define FTL_CRC32_HPP

#include "types.hpp"
#include "strings.hpp"

namespace ftl
{

class Crc32
{
public:
	Crc32(uint32_t seed = ~uint32_t(0))
		: crc_(seed)
	{}

	void feed(const void *buf, int bufFill);
	inline uint32_t sum() const { return crc_; }

private:
	uint32_t crc_;
};

inline uint32_t crc32(const void *buf, int bufSize) {
	Crc32 crc;
	crc.feed(buf, bufSize);
	return crc.sum();
}

inline uint32_t crc32(const char *s) {
	Crc32 crc;
	crc.feed(s, str::len(s));
	return crc.sum();
}

} // namespace ftl

#endif // FTL_CRC32_HPP
