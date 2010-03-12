#include <pona/stdio>
#include <pona/threads>
#include <pona/time>

namespace pona
{

int main()
{
	const int n = 100000;
	{
		Mutex m;
		Time t = now();
		for (int i = 0; i < n; ++i) {
			m.acquire();
			m.release();
		}
		t = now() - t;
		print("Took %% us\n", t.us());
	}
	/*{
		CoreMutex m;
		Time t = now();
		for (int i = 0; i < n; ++i) {
			m.acquire();
			m.release();
		}
		t = now() - t;
		print("Took %% us\n", t.us());
	}*/
	return 0;
}

} // namespace pona

int main()
{
	return pona::main();
}
