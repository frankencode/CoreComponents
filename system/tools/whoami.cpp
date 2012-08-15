#include <ftl/stdio>
#include <ftl/process>

using namespace ftl;

int main()
{
	print("%%\n", User::newInstance(Process::effectiveUserId())->loginName());
	return 0;
}
