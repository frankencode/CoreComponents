#include <ftl/stdio>
#include <ftl/process>
#include <ftl/container>

using namespace ftl;

int main()
{
	Ref<List<String>, Owner> list = new List<String>;
	while (input()->hasNext())
		list->append(input()->next());
	
	Ref<Heap<String>, Owner> heap = new Heap<String>(list->length());
	while (list->hasNext())
		heap->push(list->next());
	
	while (heap->hasNext())
		output()->writeLine(heap->next());
	
	return 0;
}
