/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUX_COLOR_H
#define FLUX_COLOR_H

#include "String.h"

namespace flux
{

class Variant;

class Color
{
public:
	static Color parse(const char *s, bool *ok = 0);

	Color(): word_(0) {}

	Color(int r, int g, int b, int a = 0xff)
	{
		rgba_[0] = r;
		rgba_[1] = g;
		rgba_[2] = b;
		rgba_[3] = a;
	}

	inline uint8_t &r() { return rgba_[0]; }
	inline uint8_t &g() { return rgba_[1]; }
	inline uint8_t &b() { return rgba_[2]; }
	inline uint8_t &a() { return rgba_[3]; }

	inline bool operator==(const Color &b) const { return word_ == b.word_; }
	inline bool operator!=(const Color &b) const { return word_ != b.word_; }

protected:
	friend class Variant;

	inline static Color cast(uint32_t x) { Color c; c.word_ = x; return c; }

	union {
		uint8_t rgba_[4];
		uint32_t word_;
	};
};

String str(Color c);

} // namespace flux

#endif // FLUX_COLOR_H
