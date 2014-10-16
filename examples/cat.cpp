#include <flux/stdio>
#include <flux/File>

using namespace flux;

int main(int argc, char **argv)
{
	if (argc > 1) {
		for (int i = 1; i < argc; ++i)
			out()->write(File::open(argv[i])->map());
	}
	else {
		out()->write(in()->readAll());
	}
	return 0;
}
