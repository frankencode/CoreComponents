#include <ftl/streams>
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
	
	FTL_CHECK(y() == 1);
	
	return 0;
}

} // namespace ftl

int main()
{
	return ftl::main();
}
