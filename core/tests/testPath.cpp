#include <flux/stdio.h>

using namespace flux;

int main(int argc, char **argv)
{
	fout("argv[0] = \"%%\"\n") << argv[0];
	fout("String(argv[0])->fileName() = \"%%\"\n") << String(argv[0])->fileName();
	fout("String(argv[0])->isAbsolutePath() = %%\n") << String(argv[0])->isAbsolutePath();
	fout("String(argv[0])->absolutePath() = \"%%\"\n") << String(argv[0])->absolutePath();
	{
		String path = String(argv[0])->absolutePath();
		while (path != "/") {
			path = path->reducePath();
			fout("path1->reduce() = \"%%\"\n") << path;
		}
	}
	{
		String path = argv[0];
		while (path->contains('/')) {
			path = path->reducePath();
			fout("path2->reduce() = \"%%\"\n") << path;
		}
	}
	return 0;
}
