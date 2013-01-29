#include <ftl/stdio>

namespace ftl
{

int main(int argc, char **argv)
{
	print("argv[0] = \"%%\"\n", argv[0]);
	print("string(argv[0])->fileName() = \"%%\"\n", string(argv[0])->fileName());
	print("string(argv[0])->isAbsolutePath() = %%\n", string(argv[0])->isAbsolutePath());
	print("string(argv[0])->absolutePath() = \"%%\"\n", string(argv[0])->absolutePath());
	{
		string path = string(argv[0])->absolutePath();
		while (path != "/") {
			path = path->reducePath();
			print("path1->reduce() = \"%%\"\n", path);
		}
	}
	{
		string path = argv[0];
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
