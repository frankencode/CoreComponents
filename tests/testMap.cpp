#include <ftl/streams>
#include <ftl/container>
#include <ftl/utils>
#include <map>

namespace ftl {

void simpleInsertTest()
{
	Map<String, String> names;
	print("%%\n", names.health());
	names.insert("Doe", "Joe");
	print("%%\n", names.health());
	names.insert("Mustermann", "Hans");
	print("%%\n", names.health());
	names.insert("Mustermann", "Max");
	print("%%\n", names.health());
	names.insert("Müller", "Otto");
	print("%%\n", names.health());
	names.insert("Berg", "Johanna");
	print("%%\n", names.health());
	names.insert("Becker", "Günther");
	print("%%\n", names.health());
	names.insert("", "X");
	print("%%\n", names.health());
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
		print("map.health() = %%\n", map.health());
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
