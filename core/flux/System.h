/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_SYSTEM_H
#define FLUX_SYSTEM_H

namespace flux
{

class System
{
public:
	static int concurrency();
	static int pageSize();
	static double now();
};

} // namespace flux

#endif // FLUX_SYSTEM_H
