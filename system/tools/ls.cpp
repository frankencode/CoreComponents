#include <ftl/stdio>
#include <ftl/process>

int main()
{
	using namespace ftl;

	Ref<Dir, Owner> dir = Dir::newInstance(".");
	for (Ref<DirEntry, Owner> entry = DirEntry::newInstance(); dir->read(entry);)
		output()->writeLine(entry->name());
	return 0;
}
