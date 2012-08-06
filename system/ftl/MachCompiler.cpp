#include "stdio"
#include "WireObject.hpp"
#include "MachCompiler.hpp"

namespace ftl
{

MachCompiler::MachCompiler(int options)
	: options_(options)
{}

bool MachCompiler::build(Ref<WireObject> recipe)
{
	Ref<WireList> sourcePaths = recipe->member("source");
	for (int i = 0, n = sourcePaths->length(); i < n; ++i) {
		Ref<DirEntry, Owner> entry = new DirEntry;
		Ref<Glob, Owner> glob = new Glob(sourcePaths->at(i));
		while (glob->read(entry)) {
			Ref<MachObject, Owner> object = analyse(entry->path());
			print("%%:", object->objectPath());
			Ref<StringList> deps = object->dependencyPaths();
			for (int j = 0, m = deps->length(); j < m; ++j)
				print(" %%", deps->at(j));
			print("\n\n");
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
