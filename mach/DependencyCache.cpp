#include <ftl/Wire.hpp>
#include <ftl/Format.hpp>
#include <ftl/File.hpp>
#include <ftl/FileStatus.hpp>
#include <ftl/PrintDebug.hpp> // DEBUG
#include "BuildPlan.hpp"
#include "DependencyCache.hpp"

namespace mach
{

O<DependencyCache> DependencyCache::create(BuildPlan *buildPlan)
{
	return new DependencyCache(buildPlan);
}

String DependencyCache::cachePath(BuildPlan *buildPlan)
{
	return buildPlan->modulePath("DependencyCache");
}

DependencyCache::DependencyCache(BuildPlan *buildPlan)
	: buildPlan_(buildPlan),
	  cachePath_(cachePath(buildPlan)),
	  cache_(Cache::create())
{
	File::establish(cachePath_);
	Time cacheTime = File::status(cachePath_)->lastModified();

	O<WireObject> dependencyCache;
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

		WireObject *wire = cast<WireObject>(item->value());
		String command = wire->value("analyseCommand");
		String modulePath = wire->value("modulePath");
		O<StringList> dependencyPaths = cast<VariantList>(wire->value("dependencyPaths"))->toList<String>();
		String sourcePath = dependencyPaths->at(0);

		bool dirty = false;

		O<FileStatus> objectStatus = buildPlan_->fileStatus(modulePath);
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
		Module *module = item->value();
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

bool DependencyCache::lookup(String source, O<Module> *module)
{
	return cache_->lookup(source, module);
}

void DependencyCache::insert(String source, Module *module)
{
	cache_->insert(source, module);
}

} // namespace mach
