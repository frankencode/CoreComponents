#include <ftl/stdio>
#include <ftl/container>

int main()
{
	using namespace ftl;

	hook<StringList> list = StringList::create();
	for (string line; input()->read(&line);)
		list->append(line);

	hook< Heap<string> > heap = Heap<string>::create(list->length());
	for (string item; list->read(&item);)
		heap->push(item);
	for (string item; heap->read(&item);)
		output()->writeLine(item);

	return 0;
}
