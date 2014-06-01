/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "ResourceContextStack.h"
#include "ResourceContext.h"

namespace flux
{

ResourceContext::ResourceContext(String resource)
{
	resourceContextStack()->push(resource);
}

ResourceContext::~ResourceContext()
{
	resourceContextStack()->pop();
}

} // namespace flux
