#include <ftl/stdio>
#include <ftl/container>
#include <ftl/utils>
#include <map>

namespace ftl {

void simpleInsertTest()
{
	Map<String, String> names;
	names.insert("Doe", "Joe");
	names.insert("Mustermann", "Hans");
	names.insert("Mustermann", "Max");
	names.insert("Müller", "Otto");
	names.insert("Berg", "Johanna");
	names.insert("Becker", "Günther");
	names.insert("", "X");
	for (int i = 0; i < names.length(); ++i) {
		Pair<String, String> pair = names.get(i);
		print("%%: %%\n", pair.key(), pair.value());
	}
}

void performanceTest()
{
	const int n = 10000;
	{
		std::map<int, int> map;
		Time t0 = Time::now();
		for (int i = 0; i < n; ++i)
			map[i] = i;
		print("std::map, %% insertions: dt = %% us\n", n, (Time::now() - t0).us());
		t0 = Time::now();
		int s = 0;
		for (std::map<int, int>::const_iterator i = map.begin(); i != map.end(); ++i)
			s += i->second;
		print("std::map, %% iteration steps: dt = %% us\n", n, (Time::now() - t0).us());
	}
	{
		ftl::Map<int, int> map;
		Time t0 = Time::now();
		for (int i = 0; i < n; ++i)
			map[i] = i;
		print("ftl::Map, %% insertions: dt = %% us\n", n, (Time::now() - t0).us());
		t0 = Time::now();
		int s = 0;
		for (int i = 0; i < n; ++i)
			s += map.get(i).value();
		print("ftl::Map, %% iteration steps: dt = %% us\n", n, (Time::now() - t0).us());
	}
}

void simpleRangeTest()
{
	Map<int, int> map;
	Random r;
	for (int i = 0; i < 20; ++i)
		map.insert(r.get(0, 100), i);
	for (int i = 0; i < map.length(); ++i)
		print("map.at(%%) = %% (%%)\n", i, map.at(i).key(), map.at(i).value());
	const int a = 30, b = 80;
	print("In range [%%..%%]:\n", a, b);
	for (int i = map.first(a), j = map.last(b); i <= j; ++i)
		print("map.at(%%) = %% (%%)\n", i, map.at(i).key(), map.at(i).value());
}

} // namespace ftl

int main()
{
	/*ftl::simpleInsertTest();
	ftl::performanceTest();*/
	ftl::simpleRangeTest();
	return 0;
}
