#include <ftl/stdio>

namespace ftl
{

int main(int argc, char** argv)
{
	print("argv[0] = \"%%\"\n", argv[0]);
	print("Path(argv[0]).fileName() = \"%%\"\n", Path(argv[0]).fileName());
	print("Path(argv[0]).isAbsolute() = %%\n", Path(argv[0]).isAbsolute());
	print("Path(argv[0]).makeAbsolute() = \"%%\"\n", Path(argv[0]).makeAbsolute());
	{
		String path = Path(argv[0]).makeAbsolute();
		while (path != "/") {
			path = Path(path).reduce();
			print("Path(path1).reduce() = \"%%\"\n", path);
		}
	}
	{
		String path = argv[0];
		while (path->contains('/')) {
			path = Path(path).reduce();
			print("Path(path2).reduce() = \"%%\"\n", path);
		}
	}
	return 0;
}

} // namespace ftl

int main(int argc, char** argv)
{
	return ftl::main(argc, argv);
}
