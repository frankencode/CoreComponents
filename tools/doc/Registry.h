/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXDOC_REGISTRY_H
#define FLUXDOC_REGISTRY_H

#include <flux/yason/yason.h>

namespace flux { template<class> class Singleton; }

namespace fluxdoc
{

using namespace flux;
using namespace flux::yason;

class Generator;

class Registry: public Object
{
public:
	Generator* generatorByName(String name) const;
	Generator* generatorByIndex(int index) const;
	int generatorCount() const;

	inline YasonProtocol *designProtocol() const { return designProtocol_; }

private:
	friend class Singleton<Registry>;
	friend class Generator;

	Registry();
	void registerGenerator(Generator *generator);

	typedef Map< String, Ref<Generator> > GeneratorByName;
	Ref<GeneratorByName> generatorByName_;

	Ref<YasonProtocol> designProtocol_;
};

Registry *registry();

} // namespace fluxdoc

#endif // FLUXDOC_REGISTRY_H
