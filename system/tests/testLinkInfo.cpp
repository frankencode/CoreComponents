#include <ftl/stdio>
#include <ftl/misc>

int main()
{
	using namespace ftl;
	Ref<LinkInfo, Owner> info = LinkInfo::newInstance();
	print("%%\n", info->libraryPath());
	print("%%\n", info->symbolName());
	print("%%.%%.%%\n", info->majorVersion(), info->minorVersion(), info->patchLevel());
	return 0;
}
