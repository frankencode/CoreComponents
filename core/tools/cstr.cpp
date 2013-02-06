#include <ftl/PrintDebug.hpp>
#include <ftl/File.hpp>

int main(int argc, char **argv)
{
	using namespace ftl;

	for (int i = 1; i < argc; ++i) {
		Ref<LineSource> source = LineSource::open(File::open(argv[i]));
		for(string line; source->read(&line);) {
			line = line->replace("\\", "\\\\");
			line = line->replace("\"", "\\\"");
			print("\"%%\\n\"\n", line);
		}
	}
	return 0;
}
