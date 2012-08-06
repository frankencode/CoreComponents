#include "MachObject.hpp"
#include "MachCompiler.hpp"
#include "Wire.hpp"
#include "Format.hpp"
#include "File.hpp"
#include "FileStatus.hpp"
#include "Set.hpp"
#include "MachDependencyCache.hpp"

namespace ftl
{

MachDependencyCache::MachDependencyCache(Ref<MachCompiler> compiler, Ref<StringList> sourcePaths, String cachePath)
	: compiler_(compiler),
	  cacheFile_(new File(cachePath)),
	  cacheTime_(cacheFile_->status()->lastModified()),
	  cache_(new Cache)
{
	Ref<WireObject, Owner> object = wire()->parse(cacheFile_->load());
	if (!object) return;
	if (object->className() != "DependencyCache") return;
	Ref< Set<String>, Owner > sourcePathsSet = new Set<String>;
	for (int i = 0; i < sourcePaths->length(); ++i)
		sourcePathsSet->insert(sourcePaths->at(i));
	for (int i = 0; i < object->length(); ++i) {
		WireObject::Item item = object->at(i);
		String sourcePath = item.key();
		Ref<VariantList> dependencyPaths = item.value();
		// Ref<MachObject, Owner> object = new MachObject(sourcePath,
		if (sourcePathsSet->contains(sourcePath)) {
			// etc..
		}
	}
}

MachDependencyCache::~MachDependencyCache()
{
	Format text;
	String indent(4, ' ');
	text << "DependencyCache {\n";
	for (int i = 0; i < cache_->length(); ++i) {
		Ref<MachObject> object = cache_->valueAt(i);
		text << indent << "\"" << object->sourcePath() << "\": [";
		Ref<StringList, Owner> dependencyPaths = object->dependencyPaths();
		for (int j = 0; j < dependencyPaths->length(); ++j)
			text << " " << dependencyPaths->at(j);
		text << "]\n";
	}
	text << "}\n";
	cacheFile_->save(text);
}

Ref<MachObject, Owner> MachDependencyCache::analyse(String sourcePath)
{
	Ref<MachObject, Owner> object;
	if (cache_->lookup(sourcePath, &object))
		if (object->sourceFile()->status()->lastModified() <= cacheTime_)
			return object;
	object = compiler_->analyse(sourcePath);
	cache_->establish(sourcePath, object);
	return object;
}

} // namespace ftl
