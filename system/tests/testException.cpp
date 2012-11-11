#include <ftl/stdio>
#include <ftl/threads>

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
		Mutex::newInstance()->release();
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
