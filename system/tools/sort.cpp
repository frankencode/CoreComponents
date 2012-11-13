#include <ftl/stdio>
#include <ftl/container>

int main()
{
	using namespace ftl;

	auto list = StringList::create();
	for (String line; input()->read(&line);)
		list->append(line);

	auto heap = Heap<String>::create(list->length());
	for (String item; list->read(&item);)
		heap->push(item);
	for (String item; heap->read(&item);)
		output()->writeLine(item);

	return 0;
}
