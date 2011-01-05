/*
 * Random.hpp -- pseuo-random numbers
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */
#ifndef FTL_RANDOM_HPP
#define FTL_RANDOM_HPP

#include "atoms"

namespace ftl
{

/** Random number generator as described by Lewis et al/1969
  * for the System/360. The generator is reentrant.
  */
class Random: public Instance
{
public:
	Random(int seed = -1);
	
	inline int max() const { return m_ - 1; }
	inline int period() const { return m_ - 1; }
	
	inline int get() {
		x_ = (16807 * x_) % m_; /* 7^5 == 16807 */
		return x_;
	}
	
	/** Return a random number in range [a, b].
	  */
	inline int get(int a, int b) {
		check(b <= m_ - 1);
		check(a <= b);
		return (uint64_t(get()) * (b - a)) / (m_ - 1) + a;
	}
	
private:
	enum { m_ = (1u << 31) - 1 };
	unsigned x_;
};

} // namespace ftl

#endif // FTL_RANDOM_HPP
