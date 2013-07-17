#include <fkit/stdio.h>
#include <fkit/Heap.h>

using namespace fkit;

int main()
{
	Ref<StringList> list = StringList::create();
	for (String item; lineInput()->read(&item);)
		list->append(item);

	list = list->sort();

	for (String item; list->read(&item);)
		fout() << item << nl;

	return 0;
}
