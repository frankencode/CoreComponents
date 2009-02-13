#include <pona/IoSyntaxSugar.hpp>

int main()
{
	pona::print("Hello, world!\n");

#ifdef PONA_WINDOWS
	pona::output()->write("\nPress <RETURN> to continue...\n");
	pona::input()->readLine();
#endif

	return 0;
}
