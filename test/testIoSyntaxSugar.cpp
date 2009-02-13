#include <pona/Core.hpp>

namespace pona
{

int main()
{
	String name, family;
	print("[Your name ?] > ");
	
	int n = scan("%% %%", &name, &family);
	
	if (n == 1)
		print("Hello %%, welcome to PONA!\n", name);
	else if (n == 2)
		print("Hello Ms. %%, welcome to PONA!\n", family);
	
	print("[Your age ?] > ");
	int age = 7;
	if (scan("%%", &age)) {
		print("(age)2 = %bin%\n", age);
		print("(age)8 = %oct%\n", age);
		print("(age)16 = %hex%\n", age);
	}
	
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
