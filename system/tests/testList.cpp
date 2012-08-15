#include <ftl/stdio>
#include <ftl/container>
#include <ftl/utils>

namespace ftl
{

template<class T>
void print(Ref<List<T>, Owner> list) {
	print("[");
	for (int i = 0; i < list->length(); ++i) {
		print("%%", list->at(i));
		if (i + 1 < list->length()) print(", ");
	}
	print("]\n");
}

int main()
{
	typedef List<int> IntList;

	{
		print("Test 1:\n");
		Ref<IntList, Owner> list = IntList::newInstance();
		list << 1 << 2 << 3;
		print(list);
		Ref<IntList, Owner> list2 = IntList::newInstance();
		list >> list2;
		print(list2);
	}
	{
		print("Test 2:\n");
		Ref<IntList, Owner> list = IntList::newInstance();
		list << 1 << 2 << 3 << 4 << 5 << 6;
		print(list);
		for (int i = 0; i < list->length();) {
			if (list->at(i) % 2 != 0)
				list->pop(i);
			else
				++i;
		}
		print(list);
		list->clear();
		print(list);
		list->append(1);
		list->append(2);
		print(list);
	}
	{
		print("Test 3:\n");
		Ref<IntList, Owner> list = IntList::newInstance();
		list << 1 << 2 << 3;
		print(list);
		int x, y, z;
		list >> x >> y >> z;
		print(list);
		print("x, y, z = %%, %%, %%\n", x, y, z);
	}
	{
		print("Test 4:\n");
		Ref<IntList, Owner> list = IntList::newInstance();
		Ref<Random, Owner> random = Random::newInstance();
		for (int i = 0; i < 10; ++i)
			list << random->get(0, 99);
		print(list);

		Ref< MinHeap<int>, Owner > heap = MinHeap<int>::newInstance(list->length());
		Ref< Stack<int>, Owner > stack = Stack<int>::newInstance(list->length());
		Ref<CircularBuffer<int>, Owner> queue = CircularBuffer<int>::newInstance(list->length());

		list >> heap >> list;
		print(list);
		list >> stack >> list;
		print(list);
		list >> queue >> list;
		print(list);
		print(list->sort());
		print(list->unique());
	}
	{
		print("Test 5:\n");
		Ref<IntList, Owner> a = IntList::newInstance();
		a << 1 << 2 << 3 << 4 << 5;
		print(a);
		print(a->clone());
	}
	{
		print("Test 6:\n");
		Ref<IntList, Owner> a = IntList::newInstance(11);
		print(a);
	}
	return 0;
}

} // namespace ftl

int main()
{
	return ftl::main();
}
