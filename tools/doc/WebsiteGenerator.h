/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXDOC_WEBSITEGENERATOR_H
#define FLUXDOC_WEBSITEGENERATOR_H

#include <flux/Singleton.h>
#include "Generator.h"

namespace fluxdoc
{

class WebsiteGeneratorRegistration;

class WebsiteGenerator: public Generator
{
public:
	virtual void run(Design *design);

private:
	friend class WebsiteGeneratorRegistration;
	WebsiteGenerator();
};

class WebsiteGeneratorRegistration: public Object
{
public:
	WebsiteGeneratorRegistration() {
		if (firstTime())
			new WebsiteGenerator;
	}
private:
	static bool firstTime() { static int count = 0; return count++ == 0; }
};

namespace { WebsiteGeneratorRegistration registration; }

} // namespace fluxdoc

#endif // FLUXDOC_WEBSITEGENERATOR_H
