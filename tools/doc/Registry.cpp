/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <flux/Singleton.h>
#include "Generator.h"
#include "Registry.h"

namespace fluxdoc
{

Registry::Registry()
	: generatorByName_(GeneratorByName::create()),
	  designProtocol_(YasonProtocol::create())
{}

void Registry::registerGenerator(Generator *generator)
{
	generatorByName_->insert(generator->name(), generator);
	designProtocol_->define(generator->prototype());
}

Generator* Registry::generatorByName(String name) const
{
	Ref<Generator> generator;
	generatorByName_->lookup(name, &generator);
	return generator;
}

Generator* Registry::generatorByIndex(int index) const
{
	return generatorByName_->valueAt(index);
}

int Registry::generatorCount() const
{
	return generatorByName_->size();
}

Registry *registry() { return Singleton<Registry>::instance(); }

} // namespace fluxdoc
