#include <pona/Core.hpp>

namespace pona
{

int main()
{
	try {
		File("non-existing").open();
	}
	catch (Exception& ex) {
		print("%%\n", ex.what());
	}

#ifdef PONA_WINDOWS
	output()->write("\nPress <RETURN> to continue...\n");
	input()->readLine();
#endif
	return 0;
}

} // namespace pona

int main()
{
	return pona::main();
}
