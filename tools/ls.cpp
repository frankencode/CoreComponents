#include <ftl/streams>
#include <ftl/process>

using namespace ftl;

int main()
{
	Dir dir(Process::cwd());
	while (dir.hasNext())
		output()->writeLine(dir.next()->name());
	return 0;
}
