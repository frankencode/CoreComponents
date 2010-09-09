#include <ftl/stdio>
#include <ftl/process>
#include <ftl/container>

using namespace ftl;

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
