#include <pona/stdio>
#include <pona/process>

namespace pona
{

int main(int argc, char** argv)
{
	Dir dir(Process::cwd());
	DirEntry entry;
	while (dir.read(&entry)) {
		print("%%\n", entry.path());
	}
	return 0;
}

} // namespace pona

int main(int argc, char** argv)
{
	return pona::main(argc, argv);
}
