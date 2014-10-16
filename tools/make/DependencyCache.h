/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXMAKE_DEPENDENCYCACHE_H
#define FLUXMAKE_DEPENDENCYCACHE_H

#include <flux/String>
#include <flux/Map>

namespace flux {
class File;
class MetaObject;
}

namespace fluxmake
{

using namespace flux;

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
