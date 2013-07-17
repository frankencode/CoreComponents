#include <fkit/stdio.h>
#include <fkit/Process.h>
#include <fkit/User.h>

using namespace fkit;

int main()
{
	fout() << User::lookup(Process::effectiveUserId())->loginName() << nl;
	return 0;
}
