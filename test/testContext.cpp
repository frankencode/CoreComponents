#include <pona/stdio>
#include <pona/context>

namespace pona
{

int main(int argc, char** argv)
{
	print("argv[0] = \"%%\"\n", argv[0]);
	print("fileName(argv[0]) = \"%%\"\n", fileName(argv[0]));
	print("isAbsolute(argv[0]) = %%\n", isAbsolute(argv[0]));
	print("makeAbsolute(argv[0]) = \"%%\"\n", makeAbsolute(argv[0]));
	{
		String path = makeAbsolute(argv[0]);
		while (path != "/") {
			path = stripComponent(path);
			print("stripComponent(path1) = \"%%\"\n", path);
		}
	}
	{
		String path = argv[0];
		while (path->contains(String("/"))) {
			path = stripComponent(path);
			print("stripComponent(path2) = \"%%\"\n", path);
		}
	}
	return 0;
}

} // namespace pona

int main(int argc, char** argv)
{
	return pona::main(argc, argv);
}
