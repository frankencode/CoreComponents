#include <ftl/streams>
#include <ftl/process>

using namespace ftl;

int main()
{
	print("%%\n", User(Process::effectiveUserId()).loginName());
	return 0;
}
