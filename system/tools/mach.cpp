#include <ftl/stdio>
#include <ftl/Config.hpp>
#include <ftl/GccCompiler.hpp>

using namespace ftl;

int main(int argc, char** argv)
{
	Ref<Config, Owner> config = new Config("Recipe");
	config->init(argc, argv);
	Ref<WireObject> wire = config->object();
	Ref<WireList> header = wire->member("header");
	for (int i = 0, n = header->length(); i < n; ++i) {
		print("%%:\n", header->at(i));
		Ref<DirEntry, Owner> entry = new DirEntry;
		Ref<Glob, Owner> glob = new Glob(header->at(i));
		while (glob->read(entry)) print("  %%\n", entry->name());
	}

	Ref<GccCompiler, Owner> compiler = new GccCompiler;
	print("compiler->machine() = %%\n", compiler->machine());
	print("compiler->execPath() = %%\n", compiler->execPath());

	return 0;
}
