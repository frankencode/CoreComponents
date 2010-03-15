#include <pona/stdio>
#include <pona/process>

namespace pona
{

int main(int argc, char** argv)
{
	Variant echo = false;
	Variant world = false;
	Variant help = false;
	Variant alpha = 1;
	Variant beta = 2.;
	
	Options options;
	options.define('e', "echo", &echo, "Print \"Hello, echo!\"");
	options.define('w', "world", &world, "Print \"Hello, world!\"");
	options.define('h', "help", &help, "Print help");
	options.define('a', "alpha", &alpha, "Pass alpha value");
	options.define('b', "beta", &beta, "Pass beta value");
	Ref<StringList, Owner> files = options.read(argc, argv);
	
	if (echo) print("Hello, echo!\n");
	if (world) print("Hello, world!\n");
	if (help) { print(options.help()); return 0; }
	print("alpha, beta = %%, %%\n", alpha, beta);
	print("alpha + beta = %%\n", int(alpha) + double(beta));
	print("files = [");
	for (StringList::Index i = files->first(); files->def(i); ++i) {
		print(" %%", files->at(i));
		if (files->def(i + 1))
			print(",");
	}
	print("]\n");
	return 0;
}

} // namespace pona

int main(int argc, char** argv)
{
	int ret = 1;
	//try {
		ret = pona::main(argc, argv);
	/*}
	catch (AnyException& ex) {
		pona::print("%%\n", ex.what());
	}*/
	return ret;
}
