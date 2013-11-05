#include <flux/stdio.h>
#include <flux/Process.h>
#include <flux/User.h>

using namespace flux;

int main()
{
	fout() << User::lookup(Process::effectiveUserId())->loginName() << nl;
	return 0;
}
