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
		auto list = IntList::create();
		list << 1 << 2 << 3;
		print(list);
		auto list2 = IntList::create();
		list >> list2;
		print(list2);
	}
	{
		print("Test 2:\n");
		auto list = IntList::create();
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
		auto list = IntList::create();
		list << 1 << 2 << 3;
		print(list);
		int x, y, z;
		list >> x >> y >> z;
		print(list);
		print("x, y, z = %%, %%, %%\n", x, y, z);
	}
	{
		print("Test 4:\n");
		auto list = IntList::create();
		auto random = Random::open();
		for (int i = 0; i < 10; ++i)
			list << random->get(0, 99);
		print(list);

		auto heap = MinHeap<int>::create(list->length());
		auto stack = Stack<int>::create(list->length());
		auto queue = CircularBuffer<int>::create(list->length());

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
		auto a = IntList::create();
		a << 1 << 2 << 3 << 4 << 5;
		print(a);
		print(a->clone());
	}
	{
		print("Test 6:\n");
		auto a = IntList::create(11);
		print(a);
	}
	return 0;
}

} // namespace ftl

int main()
{
	return ftl::main();
}
