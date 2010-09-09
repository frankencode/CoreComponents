#include <ftl/stdio>
#include <ftl/threads>

namespace ftl
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

} // namespace ftl

int main()
{
	return ftl::main();
}
