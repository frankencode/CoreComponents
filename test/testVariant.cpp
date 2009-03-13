#include <pona/stdio>
#include <pona/Variant.hpp>

namespace pona
{

int main()
{
	Variant x = 1;
	Variant y = "2.";
	Variant z = true;
	Variant a = String("abc");
	
	print("x, y, z, a = %%, \"%%\", %%, \"%%\"\n", int(x), String(y), bool(z), String(a));
	
	return 0;
}

} // namespace pona

int main()
{
	return pona::main();
}
