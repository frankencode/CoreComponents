#include <ftl/stdio>

using namespace ftl;

int main(int argc, char** argv)
{
	if (argc > 1) {
		for (int i = 1; i < argc; ++i)
			rawOutput()->write((new File(argv[i], File::Read))->readAll());
	}
	else {
		rawOutput()->write(rawInput()->readAll());
	}
	return 0;
}
