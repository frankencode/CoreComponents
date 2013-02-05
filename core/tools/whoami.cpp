#include <ftl/PrintDebug.hpp>
#include <ftl/Process.hpp>
#include <ftl/User.hpp>

using namespace ftl;

int main()
{
	print("%%\n", User::lookup(Process::effectiveUserId())->loginName());
	return 0;
}
