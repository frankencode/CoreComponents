#include <pona/stdio>
#include <pona/process>

using namespace pona;

int main()
{
	print("%%\n", User(Process::effectiveUserId()).loginName());
	return 0;
}
