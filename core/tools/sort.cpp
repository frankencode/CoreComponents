#include <ftl/stdio>
#include <ftl/container>

int main()
{
	using namespace ftl;

	hook<StringList> list = StringList::create();
	for (String line; input()->read(&line);)
		list->append(line);

	hook< Heap<String> > heap = Heap<String>::create(list->length());
	for (String item; list->read(&item);)
		heap->push(item);
	for (String item; heap->read(&item);)
		output()->writeLine(item);

	return 0;
}
