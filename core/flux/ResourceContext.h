/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_RESOURCECONTEXT_H
#define FLUX_RESOURCECONTEXT_H

#include "String.h"

namespace flux
{

class ResourceContext
{
public:
	ResourceContext(String resource);
	~ResourceContext();
};

} // namespace flux

#endif // FLUX_RESOURCECONTEXT_H
