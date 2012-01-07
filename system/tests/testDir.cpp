#include <ftl/streams>
#include <ftl/process>

namespace ftl
{

int main(int argc, char** argv)
{
	Dir dir(Process::cwd());
	for (DirEntry entry; dir.read(&entry);)
		print("%% (%%)\n", entry.path(), entry.name());
	return 0;
}

} // namespace ftl

int main(int argc, char** argv)
{
	return ftl::main(argc, argv);
}
