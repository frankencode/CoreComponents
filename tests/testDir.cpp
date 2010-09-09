#include <ftl/stdio>
#include <ftl/process>

namespace ftl
{

int main(int argc, char** argv)
{
	Dir dir(Process::cwd());
	DirEntry entry;
	while (dir.read(&entry)) {
		print("%% (%%)\n", entry.path(), entry.name());
	}
	return 0;
}

} // namespace ftl

int main(int argc, char** argv)
{
	return ftl::main(argc, argv);
}
