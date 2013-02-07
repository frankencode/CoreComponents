#include <ftl/PrintDebug.hpp>
#include <ftl/String.hpp>
#include <ftl/Heap.hpp>

int main()
{
	using namespace ftl;

	Ref<StringList> list = StringList::create();
	for (String line; input()->read(&line);)
		list->append(line);

	Ref< Heap<String> > heap = Heap<String>::create(list->length());
	for (String item; list->read(&item);)
		heap->push(item);
	for (String item; heap->read(&item);)
		output()->writeLine(item);

	return 0;
}
