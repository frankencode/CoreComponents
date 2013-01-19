 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */
#ifndef FTL_RANDOM_HPP
#define FTL_RANDOM_HPP

#include "generics.hpp"

namespace ftl
{

/** Random number generator as described by Lewis et al/1969
  * for the System/360. The generator is reentrant.
  */
class Random: public Source<int>
{
public:
	inline static Ref<Random, Owner> open(int seed = -1) {
		return new Random(seed);
	}

	inline int max() const { return m_ - 1; }
	inline int period() const { return m_ - 1; }

	inline int get() {
		x_ = (16807 * x_) % m_; /* 7^5 == 16807 */
		return x_;
	}

	/** Return a random number in range [a, b].
	  */
	inline int get(int a, int b) {
		FTL_ASSERT(b <= m_ - 1);
		FTL_ASSERT(a <= b);
		return (uint64_t(get()) * (b - a)) / (m_ - 1) + a;
	}

	inline bool read(int *x) {
		*x = get();
		return true;
	}

private:
	Random(int seed);

	enum { m_ = (1u << 31) - 1 };
	unsigned x_;
};

} // namespace ftl

#endif // FTL_RANDOM_HPP
