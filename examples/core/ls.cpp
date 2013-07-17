#include <fkit/stdio.h>
#include <fkit/Process.h>
#include <fkit/Dir.h>

using namespace fkit;

int main()
{
	Ref<Dir> dir = Dir::open(Process::cwd());
	for (String name; dir->read(&name);)
		fout() << name << nl;
	return 0;
}
