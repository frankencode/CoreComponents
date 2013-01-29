#include <ftl/stdio>
#include <ftl/variant.hpp>

namespace ftl
{

int main()
{
	variant x = 1;
	variant y = "2.";
	variant z = true;
	variant a = string("abc");

	print("x, y, z, a = %%, \"%%\", %%, \"%%\"\n", int(x), string(y), bool(z), string(a));
	print("(x < y), (x == y) = %%, %%\n", x < y, x == y);
	print("(x < z), (x == z) = %%, %%\n", x < z, x == z);
	print("(y < a), (y == a) = %%, %%\n", y < a, y == a);
	print("(variant() < x), (x == variant()) = %%, %%\n", variant() < x, x == variant());
	print("(variant() < variant()), (variant() == variant()) = %%, %%\n", variant() < variant(), variant() == variant());

	return 0;
}

} // namespace ftl

int main()
{
	return ftl::main();
}
