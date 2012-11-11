#include "Module.hpp"
#include "ToolChain.hpp"
#include "Wire.hpp"
#include "Format.hpp"
#include "File.hpp"
#include "FileStatus.hpp"
#include "DependencyCache.hpp"

namespace ftl
{

Ref<DependencyCache, Owner> DependencyCache::newInstance(Ref<BuildLine> buildLine, Ref<ToolChain> toolChain, Ref<StringList> sources, int options, Ref<StringList> includePaths, String cachePath)
{
	return new DependencyCache(buildLine, toolChain, sources, options, includePaths, cachePath);
}

DependencyCache::DependencyCache(Ref<BuildLine> buildLine, Ref<ToolChain> toolChain, Ref<StringList> sources, int options, Ref<StringList> includePaths, String cachePath)
	: buildLine_(buildLine),
	  toolChain_(toolChain),
	  cachePath_(cachePath),
	  cache_(Cache::newInstance())
{
	File::establish(cachePath);
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

		if (!sources->contains(item->key())) continue;

		Ref<WireObject> wire = item->value();
		String command = wire->value("analyseCommand");
		String modulePath = wire->value("modulePath");
		Ref<StringList, Owner> dependencyPaths = Ref<VariantList>(wire->value("dependencyPaths"))->toList<String>();
		String sourcePath = dependencyPaths->at(0);

		bool dirty = false;

		Ref<FileStatus, Owner> objectStatus = buildLine->fileStatus(modulePath);
		for (int i = 0; i < dependencyPaths->length(); ++i) {
			Time sourceTime = buildLine->fileStatus(dependencyPaths->at(i))->lastModified();
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

		if (command != toolChain->analyseCommand(sourcePath, options, includePaths)) continue;

		cache_->insert(
			item->key(),
			Module::newInstance(command, modulePath, dependencyPaths, !objectStatus->exists())
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

Ref<Module, Owner> DependencyCache::analyse(String sourcePath, int options, Ref<StringList> includePaths)
{
	Ref<Module, Owner> module;
	if (cache_->lookup(sourcePath, &module))
		return module;
	module = toolChain_->analyse(buildLine_, sourcePath, options, includePaths);
	cache_->insert(sourcePath, module);
	return module;
}

} // namespace ftl
