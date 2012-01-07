#include <ftl/streams>

using namespace ftl;

int main(int argc, char** argv)
{
	if (argc > 1) {
		for (int i = 1; i < argc; ++i)
			rawOutput()->write(File::load(argv[i]));
	}
	else {
		rawOutput()->write(rawInput()->readAll());
	}
	return 0;
}
