/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_RESOURCEGUARD_H
#define FLUX_RESOURCEGUARD_H

#include "String.h"

namespace flux
{

class ResourceGuard
{
public:
	ResourceGuard(String resource);
	~ResourceGuard();
};

} // namespace flux

#endif // FLUX_RESOURCEGUARD_H
