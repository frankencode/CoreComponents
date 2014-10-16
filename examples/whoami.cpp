#include <flux/stdio>
#include <flux/Process>
#include <flux/User>

using namespace flux;

int main()
{
	fout() << User::lookup(Process::effectiveUserId())->loginName() << nl;
	return 0;
}
