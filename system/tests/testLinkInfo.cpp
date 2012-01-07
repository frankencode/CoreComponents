#include <ftl/streams>
#include <ftl/misc>

int main()
{
	using namespace ftl;
	LinkInfo info;
	print("%%\n", info.libraryPath());
	print("%%\n", info.symbolName());
	print("%%.%%.%%\n", info.majorVersion(), info.minorVersion(), info.patchLevel());
	return 0;
}
