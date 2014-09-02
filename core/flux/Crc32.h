/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUX_CRC32_H
#define FLUX_CRC32_H

#include "types.h"
#include "strings.h"
#include "ByteArray.h"

namespace flux
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
	if (buf) crc.feed(buf, bufSize);
	return crc.sum();
}

inline uint32_t crc32(const char *s) {
	Crc32 crc;
	if (s) crc.feed(s, strlen(s));
	return crc.sum();
}

inline uint32_t crc32(ByteArray *buf) {
	Crc32 crc;
	if (buf) crc.feed(buf->bytes(), buf->count());
	return crc.sum();
}

} // namespace flux

#endif // FLUX_CRC32_H
