#include <ftl/stdio>

int main(int argc, char **argv)
{
	using namespace ftl;

	for (int i = 1; i < argc; ++i) {
		Ref<LineSource, Owner> source = LineSource::newInstance(file(argv[i], File::Read));
		for(String line; source->read(&line);) {
			line = line->replace("\\", "\\\\");
			line = line->replace("\"", "\\\"");
			print("\"%%\\n\"\n", line);
		}
	}
	return 0;
}
