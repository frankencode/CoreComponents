#include <fkit/stdio.h>
#include <fkit/check.h>
#include <fkit/Variant.h>

using namespace fkit;

int main()
{
	Variant x = 1;
	Variant y = "2.";
	Variant z = true;
	Variant a = String("abc");

	check(!(x < y));
	check(!(x == y));
	check(!(x < z));
	check(x == z);
	check(y < a);
	check(y != a);
	check(!(Variant() < x));
	check(!(x == Variant()));
	check(!(Variant() < Variant()));
	check(Variant() == Variant());

	return 0;
}
