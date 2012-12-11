/*
 * Random.hpp -- pseuo-random numbers
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
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
