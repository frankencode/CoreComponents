/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <fkit/Yason.h>
#include <fkit/Format.h>
#include <fkit/File.h>
#include <fkit/FileStatus.h>
#include "BuildPlan.h"
#include "DependencyCache.h"

namespace fmake
{

Ref<DependencyCache> DependencyCache::create(BuildPlan *plan)
{
	return new DependencyCache(plan);
}

String DependencyCache::cachePath(BuildPlan *plan)
{
	return plan->modulePath("DependencyCache");
}

DependencyCache::DependencyCache(BuildPlan *plan)
	: buildPlan_(plan),
	  cachePath_(cachePath(plan)),
	  cache_(Cache::create()),
	  previousSources_(StringList::create())
{
	File::establish(cachePath_);
	double cacheTime = File::status(cachePath_)->lastModified();

	Ref<YasonObject> dependencyCache;
	try {
		dependencyCache = Yason::parse(File::load(cachePath_));
	}
	catch (YasonException &)
	{}
	if (!dependencyCache) return;
	if (dependencyCache->className() != "DependencyCache") return;

	for (int i = 0; i < dependencyCache->size(); ++i)
	{
		YasonObject::Item item = dependencyCache->at(i);

		previousSources_->append(item->key());

		if (!plan->sources()->contains(item->key())) continue;

		YasonObject *yason = cast<YasonObject>(item->value());
		String command = yason->value("analyseCommand");
		String modulePath = yason->value("modulePath");
		Ref<StringList> dependencyPaths = cast<VariantList>(yason->value("dependencyPaths"))->toList<String>();
		String sourcePath = dependencyPaths->at(0);

		bool dirty = false;

		Ref<FileStatus> objectStatus = buildPlan_->shell()->fileStatus(modulePath);
		for (int i = 0; i < dependencyPaths->size(); ++i) {
			Ref<FileStatus> sourceStatus = buildPlan_->shell()->fileStatus(dependencyPaths->at(i));
			if (!sourceStatus->exists()) {
				dirty = true;
				break;
			}
			double sourceTime = sourceStatus->lastModified();
			if (sourceTime > cacheTime) {
				dirty = true;
				break;
			}
 			if (objectStatus->exists()) {
 				if (sourceTime > objectStatus->lastModified()) {
 					dirty = true;
 					break;
 				}
 			}
 		}

		if (dirty) continue;

		if (command != buildPlan_->toolChain()->analyseCommand(plan, sourcePath)) continue;

		cache_->insert(
			item->key(),
			Module::create(command, modulePath, dependencyPaths, !objectStatus->exists())
		);
	}
}

DependencyCache::~DependencyCache()
{
	Format text;
	String indent(4, ' ');
	text << "DependencyCache {\n";
	for (int i = 0; i < cache_->size(); ++i) {
		Cache::Item item = cache_->at(i);
		String sourcePath = item->key();
		Module *module = item->value();
		text
			<< indent << "\"" << sourcePath << "\": Module {\n"
			<< indent << indent << "analyseCommand: \"" << module->analyseCommand() << "\"\n"
			<< indent << indent << "modulePath: \"" << module->modulePath() << "\"\n"
			<< indent << indent << "dependencyPaths: [\n";
		for (int i = 0, n = module->dependencyPaths()->size(); i < n; ++i) {
			text << indent << indent << indent << "\"" << module->dependencyPaths()->at(i);
			if (i != n - 1)
				text << "\",\n";
			else
				text << "\"\n";
		}
		text << indent << indent << "]\n"
			<< indent << "}\n";
	}
	text << "}\n";
	File::save(cachePath_, text);
}

StringList *DependencyCache::previousSources() const
{
	return previousSources_;
}

bool DependencyCache::lookup(String source, Ref<Module> *module)
{
	return cache_->lookup(source, module);
}

void DependencyCache::insert(String source, Module *module)
{
	cache_->insert(source, module);
}

} // namespace fmake
