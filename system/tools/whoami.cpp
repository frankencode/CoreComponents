#include <ftl/stdio>
#include <ftl/process>

using namespace ftl;

int main()
{
	print("%%\n", User::lookup(Process::effectiveUserId())->loginName());
	return 0;
}
