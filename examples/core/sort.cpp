#include <fkit/stdio.h>
#include <fkit/Heap.h>

using namespace fkit;

int main()
{
	Ref<StringList> list = StringList::create();
	for (String item; lineInput()->read(&item);)
		list->append(item);

	list = list->sort();

	for (int i = 0; i < list->size(); ++i)
		fout() << list->at(i) << nl;

	return 0;
}
