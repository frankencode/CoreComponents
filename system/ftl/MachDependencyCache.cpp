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

MachDependencyCache::MachDependencyCache(Ref<MachCompiler> compiler, Ref<StringList> sourcePaths, String cachePath)
	: compiler_(compiler),
	  cacheFile_(File::newInstance(cachePath)),
	  cache_(Cache::newInstance())
{
	cacheFile_->establish();
	print("sourcePaths = [\n%%]\n", sourcePaths->join("\n  "));
	print("cacheFile_->exists() = %%\n", cacheFile_->exists());
	print("cacheFile_->path() = \"%%\"\n", cacheFile_->path());
	cacheTime_ = cacheFile_->status()->lastModified();

	Ref<WireObject, Owner> dependencyCache;
	try {
		dependencyCache = wire()->parse(cacheFile_->load());
	}
	catch (WireException&)
	{}
	if (!dependencyCache) return;
	if (dependencyCache->className() != "DependencyCache") return;

	for (int i = 0; i < dependencyCache->length(); ++i) {
		WireObject::Item item = dependencyCache->at(i);
		if (!sourcePaths->contains(item->key())) continue;
		Ref<WireObject> wire = item->value();
		cache_->insert(item->key(),
			new MachObject(
				wire->value("objectPath"),
				Ref<VariantList>(wire->value("dependencyPaths"))->toList<String>()

			)
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

Ref<MachObject, Owner> MachDependencyCache::analyse(String sourcePath)
{
	Ref<MachObject, Owner> object;
	if (cache_->lookup(sourcePath, &object)) {
		Ref<File, Owner> sourceFile = File::newInstance(sourcePath);
		if (sourceFile->exists()) {
			if (sourceFile->status()->lastModified() <= cacheTime_)
				return object;
		}
	}
	object = compiler_->analyse(sourcePath);
	cache_->establish(sourcePath, object);
	return object;
}

} // namespace ftl
