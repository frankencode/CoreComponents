#include <ftl/streams>
#include <ftl/threads>
#include <ftl/utils>

namespace ftl
{

int main()
{
	const int n = 1000000;
	Time t1, t2, t3;
	{
		Mutex m;
		Time t = Time::now();
		for (int i = 0; i < n; ++i) {
			m.acquire();
			m.release();
		}
		t = Time::now() - t;
		print("Took %% us\n", t.us());
		t1 = t;
	}
	{
		CoreMutex m;
		Time t = Time::now();
		for (int i = 0; i < n; ++i) {
			m.acquire();
			m.release();
		}
		t = Time::now() - t;
		print("Took %% us\n", t.us());
		t2 = t;
	}
	{
		volatile int flag = 0;
		Time t = Time::now();
		for (int i = 0; i < n; ++i) {
			++flag;
			--flag;
		}
		t = Time::now() - t;
		print("Took %% us\n", t.us());
		t3 = t;
	}
	print("Ratio: t1/t2, t2/t3 = %.3%, %.3%\n", t1/t2, t2/t3);
	return 0;
}

} // namespace ftl

int main()
{
	return ftl::main();
}
