/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "System.h"
#include "Random.h"

namespace fkit
{

Random::Random(int seed)
	: x_(seed)
{
	if (seed < 0) {
		while (x_ <= 1)
			x_ = int(System::now() * 1e9);
	}
	else {
		x_ = seed + 2;
		if (x_ <= 1) x_ += 2; // overflow check
	}
}

} // namespace fkit
