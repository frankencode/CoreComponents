#include <pona/stdio>
#include <pona/process>

int main()
{
	using namespace pona;
	
	print("%%\n", User(Process::effectiveUserId()).loginName());
	return 0;
}
