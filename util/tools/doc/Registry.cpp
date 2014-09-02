/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/Singleton.h>
#include "Generator.h"
#include "Registry.h"

namespace flux {
namespace doc {

Registry::Registry()
	: generatorByName_(GeneratorByName::create()),
	  designProtocol_(MetaProtocol::create())
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
	return generatorByName_->count();
}

Registry *registry() { return Singleton<Registry>::instance(); }

}} // namespace flux::doc
