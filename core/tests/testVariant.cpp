#include <flux/stdio.h>
#include <flux/check.h>
#include <flux/Variant.h>

using namespace flux;

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
