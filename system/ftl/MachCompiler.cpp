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
	Ref<StringList, Owner> sourcePaths = new StringList;
	{
		Format sourcePatterns = Format() << recipe->value("source");
		Ref<DirEntry, Owner> entry = new DirEntry;
		for (int i = 0; i < sourcePatterns->length(); ++i) {
			Ref<Glob, Owner> glob = new Glob(sourcePatterns->at(i));
			while (glob->read(entry))
				sourcePaths->append(entry->path());
		}
	}

	Ref<MachDependencyCache, Owner> dependencyCache = new MachDependencyCache(this, sourcePaths);

	for (int i = 0; i < sourcePaths->length(); ++i) {
		Ref<MachObject, Owner> object = dependencyCache->analyse(sourcePaths->at(i));
		print("%%: %%", object->objectPath(), object->dependencyPaths()->join(" "));
		print("\n\n");
	}

	return true;
}

void MachCompiler::init(String execPath, String machine)
{
	execPath_ = execPath;
	machine_ = machine;
}

} // namespace ftl
