#include <ftl/PrintDebug.hpp>
#include <ftl/Random.hpp>
#include <ftl/List.hpp>

namespace ftl
{

template<class T>
void print(hook< List<T> > list) {
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
		hook<IntList> list = IntList::create();
		list << 1 << 2 << 3;
		print(list);
	}
	{
		print("Test 2:\n");
		hook<IntList> list = IntList::create();
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
		hook<IntList> list = IntList::create();
		list << 1 << 2 << 3;
		print(list);
		int x, y, z;
		list >> x >> y >> z;
		print(list);
		print("x, y, z = %%, %%, %%\n", x, y, z);
	}
	{
		print("Test 4:\n");
		hook<IntList> list = IntList::create();
		hook<Random> random = Random::open();
		for (int i = 0; i < 10; ++i)
			list << random->get(0, 99);
		print(list);
		print(list->sort());
		print(list->unique());
	}
	{
		print("Test 5:\n");
		hook<IntList> a = IntList::create();
		a << 1 << 2 << 3 << 4 << 5;
		print(a);
		print(a->clone());
	}
	{
		print("Test 6:\n");
		hook<IntList> a = IntList::create(11);
		print(a);
	}
	return 0;
}

} // namespace ftl

int main()
{
	return ftl::main();
}
