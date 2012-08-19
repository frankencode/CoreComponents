#include <ftl/stdio>
#include <ftl/Config.hpp>
#include <ftl/GccCompiler.hpp>

using namespace ftl;

int main(int argc, char **argv)
{
	Ref<Config, Owner> config = Config::newInstance("Recipe");
	config->init(argc, argv);

	Ref<GccCompiler, Owner> compiler = GccCompiler::newInstance();
	return compiler->build(config->object());
}
