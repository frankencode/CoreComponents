/*
 * Random.cpp -- pseuo-random numbers
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */

#include "Time.hpp"
#include "Random.hpp"

namespace ftl
{

Random::Random(int seed)
	: x_(seed)
{
	if (seed < 0) {
		while (x_ <= 1)
			x_ = Time::now().nsec();
	}
	else {
		x_ = seed + 2;
		if (x_ <= 1) x_ += 2; // overflow check
	}
}

} // namespace ftl
