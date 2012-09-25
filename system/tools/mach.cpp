#include <ftl/stdio>
#include <ftl/Config.hpp>
#include <ftl/GccToolChain.hpp>

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

	Ref<ToolChain, Owner> toolChain = GccToolChain::newInstance();

	if (recipe->flag("c") || recipe->flag("clean")) {
		toolChain->clean(recipe);
		return 0;
	}

	if (recipe->flag("dist-clean")) {
		toolChain->distClean(recipe);
		return 0;
	}

	return toolChain->build(recipe) ? 0 : 1;
}
