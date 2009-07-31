/*
 * Random.cpp -- pseuo-random numbers
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */

#include "Time.hpp"
#include "Random.hpp"

namespace pona
{

Random::Random(int seed)
	: x_((seed >= 0) ? seed : now().nsec())
{
	if (x_ == 0)
		x_ = 1;
}

} // namespace pona
