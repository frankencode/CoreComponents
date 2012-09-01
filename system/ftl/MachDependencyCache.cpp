#include "stdio" // DEBUG
#include "MachObject.hpp"
#include "MachCompiler.hpp"
#include "Wire.hpp"
#include "Format.hpp"
#include "File.hpp"
#include "FileStatus.hpp"
#include "MachDependencyCache.hpp"

namespace ftl
{

MachDependencyCache::MachDependencyCache(Ref<MachCompiler> compiler, Ref<StringList> sourcePaths, int options, String cachePath)
	: compiler_(compiler),
	  cacheFile_(File::newInstance(cachePath)),
	  cache_(Cache::newInstance())
{
	cacheFile_->establish();
	Time cacheTime = cacheFile_->status()->lastModified();

	Ref<WireObject, Owner> dependencyCache;
	try {
		dependencyCache = wire()->parse(cacheFile_->load());
	}
	catch (WireException &)
	{}
	if (!dependencyCache) return;
	if (dependencyCache->className() != "DependencyCache") return;

	for (int i = 0; i < dependencyCache->length(); ++i)
	{
		WireObject::Item item = dependencyCache->at(i);

		if (!sourcePaths->contains(item->key())) continue;

		Ref<WireObject> wire = item->value();
		String command = wire->value("command");
		String objectPath = wire->value("objectPath");
		Ref<StringList, Owner> dependencyPaths = Ref<VariantList>(wire->value("dependencyPaths"))->toList<String>();
		String sourcePath = dependencyPaths->at(0);

		bool dirty = false;

		Time objectTime = FileStatus::newInstance(objectPath)->lastModified();
		for (int i = 0; i < dependencyPaths->length(); ++i) {
			Time sourceTime = FileStatus::newInstance(dependencyPaths->at(i))->lastModified();
			if ((sourceTime > cacheTime) || (sourceTime > objectTime)) {
				dirty = true;
				break;
			}
		}

		if (dirty) continue;

		if (command != compiler->analyseCommand(sourcePath, options)) continue;

		cache_->insert(
			item->key(),
			MachObject::newInstance(command, objectPath, dependencyPaths, false)
		);
	}
}

MachDependencyCache::~MachDependencyCache()
{
	Format text;
	String indent(4, ' ');
	text << "DependencyCache {\n";
	for (int i = 0; i < cache_->length(); ++i) {
		Cache::Item item = cache_->at(i);
		String sourcePath = item->key();
		Ref<MachObject> object = item->value();
		text
			<< indent << "\"" << sourcePath << "\": MachObject {\n"
			<< indent << indent << "command: \"" << object->command() << "\"\n"
			<< indent << indent << "objectPath: \"" << object->objectPath() << "\"\n"
			<< indent << indent << "dependencyPaths: [\n";
		for (int i = 0, n = object->dependencyPaths()->length(); i < n; ++i) {
			text << indent << indent << indent << "\"" << object->dependencyPaths()->at(i);
			if (i != n - 1)
				text << "\",\n";
			else
				text << "\"\n";
		}
		text << indent << indent << "]\n"
			<< indent << "}\n";
	}
	text << "}\n";
	cacheFile_->save(text);
}

Ref<MachObject, Owner> MachDependencyCache::analyse(String sourcePath, int options)
{
	Ref<MachObject, Owner> object;
	if (cache_->lookup(sourcePath, &object))
		return object;
	object = compiler_->analyse(sourcePath, options);
	cache_->insert(sourcePath, object);
	return object;
}

} // namespace ftl
