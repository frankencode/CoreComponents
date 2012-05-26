#include <ftl/stdio>
#include <ftl/container>
#include <ftl/utils>

namespace ftl
{

template<class T>
void print(List<T>& list) {
	print("[");
	for (int i = 0; i < list.length(); ++i) {
		print("%%", list.at(i));
		if (i + 1 < list.length()) print(", ");
	}
	print("]\n");
}

int main()
{
	/*{
		print("Test 0:\n");
		List<int> list;
		Random random;
		for (int i = 0; i < 10; ++i) {
			list.push(random.get(0, list.length()), i); //list.length()
			printf("%d\n", list.health());
			printf("----------\n");
		}
		// print("%%\n", list.health());
		// print(list);
	}*/
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
		for (int i = 0; i < list.length();) {
			if (list.at(i) % 2 != 0)
				list.pop(i);
			else
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
		CircularBuffer<int> queue(list.length());

		list >> heap >> list;
		print(list);
		list >> stack >> list;
		print(list);
		list >> queue >> list;
		print(list);
		print(*list.sort());
		print(*list.unique());
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
	{
		print("Test 6:\n");
		List<int> a;
		a << 1 << 2 << 3 << 4 << 5;
		List<int> b(a);
		print(a);
		print(b);
	}
	return 0;
}

} // namespace ftl

int main()
{
	return ftl::main();
}
