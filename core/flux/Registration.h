/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_REGISTRATION_H
#define FLUX_REGISTRATION_H

namespace flux {

template<class Asset>
class Registration
{
public:
	Registration() {
		static bool firstTime = true;
		if (firstTime) new Asset;
		firstTime = false;
	}
};

} // namespace flux

#endif // FLUX_REGISTRATION_H
