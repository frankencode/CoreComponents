#include <ftl/PrintDebug.hpp>
#include <ftl/File.hpp>
#include <ftl/Mutex.hpp>

namespace ftl
{

int y() {
	print("y() called.\n");
	return 1;
}

int main()
{
	try {
		File::open("non-existing");
	}
	catch (Exception &ex) {
		print("%%\n", ex.what());
	}

	try {
		Mutex::create()->release();
	}
	catch (Exception &ex) {
		print("%%\n", ex.what());
	}

	FTL_ASSERT(y() == 1);

	return 0;
}

} // namespace ftl

int main()
{
	return ftl::main();
}
