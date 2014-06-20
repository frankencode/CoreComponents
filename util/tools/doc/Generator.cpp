/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "Registry.h"
#include "Generator.h"

namespace fluxdoc
{

Generator::Generator(String name, Design *prototype)
	: name_(name),
	  prototype_(prototype)
{
	registry()->registerGenerator(this);
}

} // namespace fluxdoc
