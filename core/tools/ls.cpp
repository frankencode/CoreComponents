#include <ftl/PrintDebug.hpp>
#include <ftl/Glob.hpp>
#include <ftl/Dir.hpp>

int main(int argc, char **argv)
{
	using namespace ftl;

	if (argc > 1) {
		for (int i = 1; i < argc; ++i) {
			Ref<Glob> glob = Glob::open(argv[i]);
			for (string path; glob->read(&path);)
				output()->writeLine(path);
		}
	}
	else {
		Ref<Dir> dir = Dir::open(".");
		for (string name; dir->read(&name);)
			output()->writeLine(name);
	}

	return 0;
}
