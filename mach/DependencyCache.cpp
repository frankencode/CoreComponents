#include <ftl/Wire.hpp>
#include <ftl/format.hpp>
#include <ftl/File.hpp>
#include <ftl/FileStatus.hpp>
#include <ftl/PrintDebug.hpp> // DEBUG
#include "BuildPlan.hpp"
#include "DependencyCache.hpp"

namespace mach
{

hook<DependencyCache> DependencyCache::create(BuildPlan *buildPlan)
{
	return new DependencyCache(buildPlan);
}

string DependencyCache::cachePath(BuildPlan *buildPlan)
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

	hook<WireObject> dependencyCache;
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
		string command = wire->value("analyseCommand");
		string modulePath = wire->value("modulePath");
		hook<StringList> dependencyPaths = cast<VariantList>(wire->value("dependencyPaths"))->toList<string>();
		string sourcePath = dependencyPaths->at(0);

		bool dirty = false;

		hook<FileStatus> objectStatus = buildPlan_->fileStatus(modulePath);
		for (int i = 0; i < dependencyPaths->length(); ++i) {
			hook<FileStatus> sourceStatus = buildPlan_->fileStatus(dependencyPaths->at(i));
			if (!sourceStatus->exists()) {
				dirty = true;
				break;
			}
			Time sourceTime = sourceStatus->lastModified();
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
	format text;
	string indent(4, ' ');
	text << "DependencyCache {\n";
	for (int i = 0; i < cache_->length(); ++i) {
		Cache::Item item = cache_->at(i);
		string sourcePath = item->key();
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

bool DependencyCache::lookup(string source, hook<Module> *module)
{
	return cache_->lookup(source, module);
}

void DependencyCache::insert(string source, Module *module)
{
	cache_->insert(source, module);
}

} // namespace mach
