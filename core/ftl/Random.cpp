/*
 * Random.cpp -- pseuo-random numbers
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
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
