#include <pona/stdio>
#include <pona/thread>
#include <pona/time>

namespace pona
{

int main()
{
	Mutex m;
	Time t = now();
	for (int i = 0; i < 1000000; ++i) {
		m.acquire();
		m.release();
	}
	t = now() - t;
	print("Took %% ms\n", t.ms());
	return 0;
}

} // namespace pona

int main()
{
	return pona::main();
}

