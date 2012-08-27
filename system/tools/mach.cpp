#include <ftl/stdio>
#include <ftl/Config.hpp>
#include <ftl/GccCompiler.hpp>

using namespace ftl;

int main(int argc, char **argv)
{
	Ref<Config, Owner> config = Config::newInstance();
	config->read("Recipe");
	config->read(argc, argv);

	if (config->contains("h") || config->contains("help")) {
		print("no help, yet...\n");
		return 0;
	}

	Ref<GccCompiler, Owner> compiler = GccCompiler::newInstance();
	return compiler->build(config);
}
