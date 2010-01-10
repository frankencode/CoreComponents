/*
 * Random.cpp -- pseuo-random numbers
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../LICENSE for the license.
 */

#include "Time.hpp"
#include "Random.hpp"

namespace pona
{

Random::Random(int seed)
	: x_(seed)
{
	if (seed < 0) {
		while (x_ <= 1)
			x_ = now().nsec();
	}
	else {
		x_ = seed + 2;
		if (x_ <= 1) x_ += 2; // overflow check
	}
}

} // namespace pona
