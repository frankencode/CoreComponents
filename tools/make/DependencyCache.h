/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXMAKE_DEPENDENCYCACHE_H
#define FLUXMAKE_DEPENDENCYCACHE_H

#include <flux/String.h>
#include <flux/Map.h>

namespace flux {
class File;
namespace yason {
class YasonObject;
}}

namespace fluxmake
{

using namespace flux;
using namespace flux::yason;

class Module;
class BuildPlan;

class DependencyCache: public Object
{
public:
	static Ref<DependencyCache> create(BuildPlan *plan);
	~DependencyCache();

	static String cachePath(BuildPlan *plan);

	StringList *previousSources() const;

	bool lookup(String source, Ref<Module> *module);
	void insert(String source, Module *module);

private:
	DependencyCache(BuildPlan *plan);

	Ref<BuildPlan> buildPlan_;
	String cachePath_;
	typedef Map< String, Ref<Module> > Cache;
	Ref<Cache> cache_;

	Ref<StringList> previousSources_;
};

} // namespace fluxmake

#endif // FLUXMAKE_DEPENDENCYCACHE_H
