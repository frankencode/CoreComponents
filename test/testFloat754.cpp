#include <pona/Core.hpp>

namespace pona
{

int main()
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
		output()->write(format("%%|\n") % values[i]);
	
	double one, zero;
	one = 1.;
	zero = 0.;
	
	output()->write(format("%%|\n") % (one/zero));
	output()->write(format("%%|\n") % (-one/zero));
	output()->write(format("%%|\n") % (zero/zero));
	output()->write(format("%%|\n") % 0.);
	
	output()->write(format("bin(zero/zero) = %hex%\n") % union_cast<uint64_t>(zero/zero));
	output()->write(format("bin(nan) = %hex%\n") % union_cast<uint64_t>(zero/zero));
	output()->write(format("unsignedMax, intMax = %%, %%\n") % unsignedMax % intMax);
	
#ifdef PONA_WINDOWS
	output()->write("\nPress <RETURM> to continue...\n");
	input()->readLine();
#endif
	return 0;
}

} // namespace pona

int main()
{
	return pona::main();
}
