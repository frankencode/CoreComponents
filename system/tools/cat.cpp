#include <ftl/stdio>

int main(int argc, char **argv)
{
	using namespace ftl;

	if (argc > 1) {
		for (int i = 1; i < argc; ++i)
			rawOutput()->write(File::open(argv[i])->readAll());
	}
	else {
		rawOutput()->write(rawInput()->readAll());
	}
	return 0;
}
