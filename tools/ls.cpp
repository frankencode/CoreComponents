#include <pona/stdio>
#include <pona/process>
#include <pona/container>

namespace ls
{

using namespace pona;

int main()
{
	Dir dir(Process::cwd());
	
	typedef List< Ref<DirEntry, Owner> > Entries;
	Ref<Entries, Owner> entries = new Entries;
	
	while (true) {
		Ref<DirEntry, Owner> entry = new DirEntry;
		if (!dir.read(entry)) break;
		entries->append(entry);
	}
	
	Heap<Entries::Item, Ascending>(entries->length()) << *entries >> *entries;
	
	for (Entries::Index i = entries->first(); entries->def(i); ++i)
		print("%%\n", entries->at(i)->name());
	
	return 0;
}

} // namespace ls

int main()
{
	return ls::main();
}
