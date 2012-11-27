#include "Module.hpp"
#include "ToolChain.hpp"
#include "Wire.hpp"
#include "Format.hpp"
#include "File.hpp"
#include "FileStatus.hpp"
#include "BuildPlan.hpp"
#include "DependencyCache.hpp"

namespace ftl
{

Ref<DependencyCache, Owner> DependencyCache::create(Ref<BuildPlan> buildPlan)
{
	return new DependencyCache(buildPlan);
}

String DependencyCache::cachePath(Ref<BuildPlan> buildPlan)
{
	return buildPlan->objectPath("DependencyCache");
}

DependencyCache::DependencyCache(Ref<BuildPlan> buildPlan)
	: buildPlan_(buildPlan),
	  cachePath_(cachePath(buildPlan)),
	  cache_(Cache::create())
{
	File::establish(cachePath_);
	Time cacheTime = File::status(cachePath_)->lastModified();

	Ref<WireObject, Owner> dependencyCache;
	try {
		dependencyCache = wire()->parse(File::load(cachePath_));
	}
	catch (WireException &)
	{}
	if (!dependencyCache) return;
	if (dependencyCache->className() != "DependencyCache") return;

	for (int i = 0; i < dependencyCache->length(); ++i)
	{
		WireObject::Item item = dependencyCache->at(i);

		if (!buildPlan->sources()->contains(item->key())) continue;

		Ref<WireObject> wire = item->value();
		String command = wire->value("analyseCommand");
		String modulePath = wire->value("modulePath");
		Ref<StringList, Owner> dependencyPaths = Ref<VariantList>(wire->value("dependencyPaths"))->toList<String>();
		String sourcePath = dependencyPaths->at(0);

		bool dirty = false;

		Ref<FileStatus, Owner> objectStatus = buildPlan_->fileStatus(modulePath);
		for (int i = 0; i < dependencyPaths->length(); ++i) {
			Time sourceTime = buildPlan_->fileStatus(dependencyPaths->at(i))->lastModified();
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

		if (command != buildPlan_->toolChain()->analyseCommand(buildPlan, sourcePath)) continue;

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
	for (int i = 0; i < cache_->length(); ++i) {
		Cache::Item item = cache_->at(i);
		String sourcePath = item->key();
		Ref<Module> module = item->value();
		text
			<< indent << "\"" << sourcePath << "\": Module {\n"
			<< indent << indent << "analyseCommand: \"" << module->analyseCommand() << "\"\n"
			<< indent << indent << "modulePath: \"" << module->modulePath() << "\"\n"
			<< indent << indent << "dependencyPaths: [\n";
		for (int i = 0, n = module->dependencyPaths()->length(); i < n; ++i) {
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

Ref<Module, Owner> DependencyCache::analyse(String source)
{
	Ref<Module, Owner> module;
	if (cache_->lookup(source, &module))
		return module;
	module = buildPlan_->toolChain()->analyse(buildPlan_, source);
	cache_->insert(source, module);
	return module;
}

} // namespace ftl
