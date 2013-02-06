#include <ftl/PrintDebug.hpp>
#include <ftl/LinkInfo.hpp>

int main()
{
	using namespace ftl;
	Ref<LinkInfo> info = LinkInfo::create();
	print("%%\n", info->libraryPath());
	print("%%\n", info->symbolName());
	print("%%.%%.%%\n", info->majorVersion(), info->minorVersion(), info->patchLevel());
	return 0;
}
