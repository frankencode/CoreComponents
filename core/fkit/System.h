/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FKIT_SYSTEM_H
#define FKIT_SYSTEM_H

namespace fkit
{

class System
{
public:
	static int concurrency();
	static int pageSize();
	static double now();
};

} // namespace fkit

#endif // FKIT_SYSTEM_H
