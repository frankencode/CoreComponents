#include <flux/stdio>
#include <flux/Process>
#include <flux/Dir>

using namespace flux;

int main()
{
	Ref<Dir> dir = Dir::open(Process::cwd());
	for (String name; dir->read(&name);)
		fout() << name << nl;
	return 0;
}
