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
	Entries entries;
	entries << dir;
	
	Heap<Entries::Item> heap(entries.length(), SortOrder::Ascending);
	entries >> heap;
	
	while (heap.hasNext())
		print("%%\n", heap.next()->name());
		
	return 0;
}

} // namespace ls

int main()
{
	return ls::main();
}
