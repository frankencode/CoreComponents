#include <ftl/PrintDebug.hpp>
#include <ftl/SpinLock.hpp>
#include <ftl/Mutex.hpp>
#include <ftl/Token.hpp>

namespace ftl
{

void testFloat754()
{
	double values[] = {
		-1./3.,
		-0.55,
		0.49,
		15,
		-1.5,
		1.1111111111,
		1.1111111111111111111111111111111,
		1e-16,
		1e-308
	};

	for (unsigned i = 0; i < sizeof(values)/sizeof(double); ++i)
		print("%%|\n", values[i]);

	double one, zero;
	one = 1.;
	zero = 0.;

	print("%%|\n", one/zero);
	print("%%|\n", -one/zero);
	print("%%|\n", zero/zero);
	print("%%|\n", 0.);

	print("bin(zero/zero) = %hex%\n", union_cast<uint64_t>(zero/zero));
	print("bin(one/zero) = %hex%\n", union_cast<uint64_t>(one/zero));
	print("bin(nan) = %hex%\n", union_cast<uint64_t>(zero/zero));
	print("unsignedMax, intMax = %%, %%\n", unsignedMax, intMax);

	print("sizeof(Instance) = %%\n", int(sizeof(Instance)));
	print("sizeof(SpinLock) = %%\n", int(sizeof(SpinLock)));
	print("sizeof(Mutex) = %%\n", int(sizeof(Mutex)));
	print("sizeof(hook<Instance>) = %%\n", int(sizeof(hook<Instance>)));
	print("sizeof(string) = %%\n", int(sizeof(string)));
	print("sizeof(format) = %%\n", int(sizeof(format)));
	print("sizeof(variant) = %%\n", int(sizeof(variant)));
	print("sizeof(Token) = %%\n", sizeof(Token));

	print("sizeof(time_t) = %%\n", sizeof(time_t));
	timeval tv;
	timespec ts;
	print("sizeof(timeval.tv_sec) = %%\n", sizeof(tv.tv_sec));
	print("sizeof(timespec.ts_sec) = %%\n", sizeof(ts.tv_sec));
}

int main()
{
	print("sizeof(void *) = %%\n", int(sizeof(void *)));
	print("sizeof(int) = %%\n", int(sizeof(int)));
	print("sizeof(long) = %%\n", int(sizeof(long)));
	print("sizeof(uint8_t) = %%\n", int(sizeof(uint8_t)));
	print("sizeof(uint16_t) = %%\n", int(sizeof(uint16_t)));
	print("sizeof(uint32_t) = %%\n", int(sizeof(uint32_t)));
	print("sizeof(uint64_t) = %%\n", int(sizeof(uint64_t)));
	print("sizeof(float32_t) = %%\n", int(sizeof(float32_t)));
	print("sizeof(float64_t) = %%\n", int(sizeof(float64_t)));
	print("------\n");
	testFloat754();
	return 0;
}

} // namespace ftl

int main()
{
	return ftl::main();
}
