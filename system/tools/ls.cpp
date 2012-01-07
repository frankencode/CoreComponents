#include <ftl/streams>
#include <ftl/process>

using namespace ftl;

int main()
{
	Dir dir(Process::cwd());
	for (DirEntry entry; dir.read(&entry);)
		output()->writeLine(entry.name());
	return 0;
}
