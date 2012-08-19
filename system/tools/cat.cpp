#include <ftl/stdio>

int main(int argc, char **argv)
{
	using namespace ftl;

	if (argc > 1) {
		for (int i = 1; i < argc; ++i) {
			Ref<File, Owner> file = File::newInstance(argv[i]);
			file->open(File::Read);
			rawOutput()->write(file->readAll());
		}
	}
	else {
		rawOutput()->write(rawInput()->readAll());
	}
	return 0;
}
