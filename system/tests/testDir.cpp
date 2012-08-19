#include <ftl/stdio>
#include <ftl/process>

namespace ftl
{

int main(int argc, char **argv)
{
	Ref<Dir, Owner> dir = Dir::newInstance(Process::cwd());
	for (Ref<DirEntry, Owner> entry = DirEntry::newInstance(); dir->read(entry);)
		print("%% (%%)\n", entry->path(), entry->name());
	return 0;
}

} // namespace ftl

int main(int argc, char **argv)
{
	return ftl::main(argc, argv);
}
