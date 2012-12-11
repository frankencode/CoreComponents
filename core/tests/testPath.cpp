#include <ftl/stdio>

namespace ftl
{

int main(int argc, char **argv)
{
	print("argv[0] = \"%%\"\n", argv[0]);
	print("String(argv[0])->fileName() = \"%%\"\n", String(argv[0])->fileName());
	print("String(argv[0])->isAbsolutePath() = %%\n", String(argv[0])->isAbsolutePath());
	print("String(argv[0])->absolutePath() = \"%%\"\n", String(argv[0])->absolutePath());
	{
		String path = String(argv[0])->absolutePath();
		while (path != "/") {
			path = path->reducePath();
			print("path1->reduce() = \"%%\"\n", path);
		}
	}
	{
		String path = argv[0];
		while (path->contains('/')) {
			path = path->reducePath();
			print("path2->reduce() = \"%%\"\n", path);
		}
	}
	return 0;
}

} // namespace ftl

int main(int argc, char **argv)
{
	return ftl::main(argc, argv);
}
