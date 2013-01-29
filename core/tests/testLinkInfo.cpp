#include <ftl/stdio>
#include <ftl/misc>

int main()
{
	using namespace ftl;
	hook<LinkInfo> info = LinkInfo::create();
	print("%%\n", info->libraryPath());
	print("%%\n", info->symbolName());
	print("%%.%%.%%\n", info->majorVersion(), info->minorVersion(), info->patchLevel());
	return 0;
}
