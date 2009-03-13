#include <pona/stdio>
#include <pona/misc>

namespace pona
{

int main(int argc, char** argv)
{
	output()->writeLine(
		format("\"%5.4%|%5.4%|%5.4%|%5.4%|%5.4%|%5.4%|%5.4%|%5.4%|%5.4%\"")
		% -1.234e10 % 12.34 % 1e9 % -1e-9 % -2. % 111.111 % 0. % -0. % nan
	);

	output()->writeLine(
		format("\"%hex%|%%|%%|%%|%%|%%|%%|%%|%%\"")
		% -1.234e10 % 12.34 % -1e6 % -1e-9 % -2. % 111.111 % 0. % -0. % nan
	);

	output()->writeLine(
		format("\"%dec:10%, %hex%, %10%, %%, %%\"")
		% intMax % -0xABCDEF % "trara" % true % false
	);
	
	struct S { int a,b,c; } v, *pv = &v;
	output()->writeLine(format("pv = %hex%") % (void*)pv);
	
	Random random(1);
	
	for (int i = 0; i < 10/*0000*/; ++i)
	{
		int r = random.next()/* % 100*/;
		output()->writeLine(format("%dec%, %hex%, %oct%, %bin%") % r % r % r % r);
	}
	
	for (int i = 0; i < 10; ++i) {
		int sign = -2 * (random.next() < random.next()) + 1;
		output()->writeLine(format("%3.3%") % (-sign * double(random.next()) / random.next()));
	}
	
	return 0;
}

} // namespace pona

int main(int argc, char** argv)
{
	return pona::main(argc, argv);
}
