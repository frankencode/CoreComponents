#include <ftl/stdio>

int main(int argc, char** argv)
{
	using namespace ftl;

	for (int i = 1; i < argc; ++i) {
		Ref<File, Owner> file = File::newInstance(argv[i], File::Read);
		Ref<LineSource, Owner> source = new LineSource(file);
		for(String line; source->read(&line);) {
			line = line->replace("\\", "\\\\");
			line = line->replace("\"", "\\\"");
			print("\"%%\\n\"\n", line);
		}
	}
	return 0;
}
