#include <ftl/PrintDebug.hpp>
#include <ftl/string.hpp>
#include <ftl/Heap.hpp>

int main()
{
	using namespace ftl;

	Ref<StringList> list = StringList::create();
	for (string line; input()->read(&line);)
		list->append(line);

	Ref< Heap<string> > heap = Heap<string>::create(list->length());
	for (string item; list->read(&item);)
		heap->push(item);
	for (string item; heap->read(&item);)
		output()->writeLine(item);

	return 0;
}
