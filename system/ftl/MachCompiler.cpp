#include "stdio"
#include "WireObject.hpp"
#include "MachDependencyCache.hpp"
#include "MachCompiler.hpp"

namespace ftl
{

MachCompiler::MachCompiler(int options)
	: options_(options)
{}

bool MachCompiler::build(Ref<WireObject> recipe)
{
	Ref<StringList, Owner> sources = StringList::newInstance();
	{
		Ref<VariantList> sourcePatterns = recipe->value("source");
		Ref<DirEntry, Owner> entry = DirEntry::newInstance();
		for (int i = 0; i < sourcePatterns->length(); ++i) {
			Ref<Glob, Owner> glob = Glob::newInstance(sourcePatterns->at(i));
			while (glob->read(entry))
				sources->append(entry->path());
		}
	}

	Ref<MachObjectList, Owner> objects = MachObjectList::newInstance(sources->length());
	{
		Ref<MachDependencyCache, Owner> dependencyCache = MachDependencyCache::newInstance(this, sources);
		for (int i = 0; i < sources->length(); ++i) {
			Ref<MachObject, Owner> object = dependencyCache->analyse(sources->at(i));
			objects->set(i, object);
			// print("%%: %%\n\n", object->objectPath(), object->dependencyPaths()->join(" "));
		}
	}


	for (int i = 0; i < objects->length(); ++i) {
		Ref<MachObject> object = objects->at(i);
		if (object->dirty()) {
			if (!compile(object)) return false;
		}
	}

	return true;
}

void MachCompiler::init(String execPath, String machine)
{
	execPath_ = execPath;
	machine_ = machine;
}

} // namespace ftl
