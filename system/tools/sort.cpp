#include <ftl/stdio>
#include <ftl/container>

int main()
{
	using namespace ftl;
	
	Ref<List<String>, Owner> list = new List<String>;
	for (String line; input()->read(&line);)
		list->append(line);
	
	Ref<Heap<String>, Owner> heap = new Heap<String>(list->length());
	for (String item; list->read(&item);)
		heap->push(item);
	for (String item; heap->read(&item);)
		output()->writeLine(item);
	
	return 0;
}
