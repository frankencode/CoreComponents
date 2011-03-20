#include <ftl/streams>

using namespace ftl;

int main(int argc, char** argv)
{
	for (int i = 1; i < argc; ++i) {
		Ref<File, Owner> file = new File(argv[i], File::Read);
		Ref<LineSource, Owner> source = new LineSource(file);
		for(String line; source->read(&line);) {
			line = line.replace("\\", "\\\\");
			line = line.replace("\"", "\\\"");
			print("\"%%\\n\"\n", line);
		}
	}
	return 0;
}
