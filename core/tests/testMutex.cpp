#include <ftl/PrintDebug.hpp>
#include <ftl/Mutex.hpp>
#include <ftl/SpinLock.hpp>
#include <ftl/System.hpp>

namespace ftl
{

int main()
{
	const int n = 1000000;
	double t1, t2, t3;
	{
		Ref<Mutex> m = Mutex::create();
		double t = System::now();
		for (int i = 0; i < n; ++i) {
			m->acquire();
			m->release();
		}
		t = System::now() - t;
		print("Took %% us\n", t * 1e6);
		t1 = t;
	}
	{
		SpinLock m;
		double t = System::now();
		for (int i = 0; i < n; ++i) {
			m.acquire();
			m.release();
		}
		t = System::now() - t;
		print("Took %% us\n", t * 1e6);
		t2 = t;
	}
	{
		volatile int flag = 0;
		double t = System::now();
		for (int i = 0; i < n; ++i) {
			++flag;
			--flag;
		}
		t = System::now() - t;
		print("Took %% us\n", t * 1e6);
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
