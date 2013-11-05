#include <flux/stdio.h>
#include <flux/Process.h>
#include <flux/Dir.h>

using namespace flux;

int main()
{
	Ref<Dir> dir = Dir::open(Process::cwd());
	for (String name; dir->read(&name);)
		fout() << name << nl;
	return 0;
}
