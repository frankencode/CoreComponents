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
	virtual void run();

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
