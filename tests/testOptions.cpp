#include <ftl/stdio>
#include <ftl/process>

namespace ftl
{

int main(int argc, char** argv)
{
	CommandLine options;
	Ref<CommandOption> echo  = options.define('e', "echo",  false, "Print \"Hello, echo!\"");
	Ref<CommandOption> world = options.define('w', "world", false, "Print \"Hello, world!\"");
	Ref<CommandOption> help  = options.define('h', "help",  false, "Print help");
	Ref<CommandOption> alpha = options.define('a', "alpha", 1,     "Pass alpha value");
	Ref<CommandOption> beta  = options.define('b', "beta",  2.,    "Pass beta value");
	
	Ref<StringList, Owner> files = options.read(argc, argv);
	
	if (echo->value()) print("Hello, echo!\n");
	if (world->value()) print("Hello, world!\n");
	if (help->value()) {
		print(options.helpText());
		return 0;
	}
	
	print("alpha, beta = %%, %%\n", alpha->value(), beta->value());
	print("alpha + beta = %%\n", int(alpha->value()) + double(beta->value()));
	print("files = [");
	for (StringList::Index i = files->first(); files->def(i); ++i) {
		print(" %%", files->at(i));
		if (files->def(i + 1))
			print(",");
	}
	print("]\n");
	
	return 0;
}

} // namespace ftl

int main(int argc, char** argv)
{
	int ret = 1;
	//try {
		ret = ftl::main(argc, argv);
	/*}
	catch (AnyException& ex) {
		ftl::print("%%\n", ex.what());
	}*/
	return ret;
}
