#include <ftl/stdio>
#include <ftl/Variant.hpp>

namespace ftl
{

int main()
{
	Variant x = 1;
	Variant y = "2.";
	Variant z = true;
	Variant a = String("abc");
	
	print("x, y, z, a = %%, \"%%\", %%, \"%%\"\n", int(x), String(y), bool(z), String(a));
	print("(x < y), (x == y) = %%, %%\n", x < y, x == y);
	print("(x < z), (x == z) = %%, %%\n", x < z, x == z);
	print("(y < a), (y == a) = %%, %%\n", y < a, y == a);
	print("(Variant() < x), (x == Variant()) = %%, %%\n", Variant() < x, x == Variant());
	print("(Variant() < Variant()), (Variant() == Variant()) = %%, %%\n", Variant() < Variant(), Variant() == Variant());
	
	return 0;
}

} // namespace ftl

int main()
{
	return ftl::main();
}
