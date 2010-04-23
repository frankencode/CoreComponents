#include <pona/stdio>
#include <pona/threads>
#include <pona/time>

namespace pona
{

int main()
{
	const int n = 1000000;
	Time t1, t2, t3;
	{
		Mutex m;
		Time t = now();
		for (int i = 0; i < n; ++i) {
			m.acquire();
			m.release();
		}
		t = now() - t;
		print("Took %% us\n", t.us());
		t1 = t;
	}
	{
		CoreMutex m;
		Time t = now();
		for (int i = 0; i < n; ++i) {
			m.acquire();
			m.release();
		}
		t = now() - t;
		print("Took %% us\n", t.us());
		t2 = t;
	}
	{
		volatile int flag = 0;
		Time t = now();
		for (int i = 0; i < n; ++i) {
			++flag;
			--flag;
		}
		t = now() - t;
		print("Took %% us\n", t.us());
		t3 = t;
	}
	print("Ratio: t1/t2, t2/t3 = %.3%, %.3%\n", t1/t2, t2/t3);
	return 0;
}

} // namespace pona

int main()
{
	return pona::main();
}
