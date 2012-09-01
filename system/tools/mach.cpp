#include <ftl/stdio>
#include <ftl/Config.hpp>
#include <ftl/GccCompiler.hpp>

using namespace ftl;

int main(int argc, char **argv)
{
	Ref<Config, Owner> recipe = Config::newInstance();
	recipe->read("Recipe");
	recipe->read(argc, argv);

	if (recipe->flag("h") || recipe->flag("help")) {
		print("no help, yet...\n");
		return 0;
	}

	Ref<GccCompiler, Owner> compiler = GccCompiler::newInstance();

	if (recipe->flag("c") || recipe->flag("clean")) {
		compiler->clean(recipe);
		return 0;
	}

	return compiler->build(recipe) ? 0 : 1;
}
