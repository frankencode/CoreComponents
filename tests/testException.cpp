#include <pona/stdio>
#include <pona/threads>

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
	
	try {
		Mutex m;
		m.release();
	}
	catch (Exception& ex) {
		print("%%\n", ex.what());
	}

	return 0;
}

} // namespace pona

int main()
{
	return pona::main();
}
