#include <pona/stdio>
#include <pona/container>
#include <pona/math>

namespace pona
{

template<class T>
void print(List<T>& list) {
	List<int>::Index i = list.first();
	print("[");
	while (list.def(i)) {
		print("%%", list.at(i));
		++i;
		if (list.def(i)) print(", ");
	}
	print("]\n");
}

int main()
{
	{
		print("Test 1:\n");
		List<int> list;
		list << 1 << 2 << 3;
		print(list);
		List<int> list2;
		list >> list2;
		print(list2);
	}
	{
		print("Test 2:\n");
		List<int> list;
		list << 1 << 2 << 3 << 4 << 5 << 6;
		print(list);
		List<int>::Index i = list.first();
		while (list.def(i)) {
			if (list.at(i) % 2 != 0)
				list.pop(i);
			++i;
		}
		print(list);
		list.clear();
		print(list);
		list.append(1);
		list.append(2);
		print(list);
	}
	{
		print("Test 3:\n");
		List<int> list;
		list << 1 << 2 << 3;
		print(list);
		int x, y, z;
		list >> x >> y >> z;
		print(list);
		print("x, y, z = %%, %%, %%\n", x, y, z);
	}
	{
		print("Test 4:\n");
		List<int> list;
		Random random;
		for (int i = 0; i < 10; ++i)
			list << random.get(0, 99);
		print(list);
		
		MinHeap<int> heap(list.length());
		Stack<int> stack(list.length());
		Queue<int> queue(list.length());
		
		list >> heap >> list;
		print(list);
		list >> stack >> list;
		print(list);
		list >> queue >> list;
		print(list);
	}
	/*{
		print("Test 5:\n");
		List< Pair<int, int> > list;
		Random random;
		for (int i = 0; i < 10; ++i)
			list << Pair<int, int>(random.get(0, 99), i);
		print("#list = %%\n", list.length());
		Map<int, int> map;
		list >> map >> list;
		print("#list = %%\n", list.length());
		print("[");
		for (List< Pair<int, int> >::Index i = list.first(); list.def(i); ++i) {
			print("(%%, %%)", list.at(i).key(), list.at(i).value());
			if (list.def(i+1)) print(", ");
		}
		print("]\n");
	}*/
	return 0;
}

} // namespace pona

int main()
{
	return pona::main();
}
