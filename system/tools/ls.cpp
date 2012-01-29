#include <ftl/stdio>
#include <ftl/process>

int main()
{
	using namespace ftl;

	Dir dir(Process::cwd());
	for (DirEntry entry; dir.read(&entry);)
		output()->writeLine(entry.name());
	return 0;
}
