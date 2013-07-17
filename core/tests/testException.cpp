#include <fkit/stdio.h>
#include <fkit/check.h>
#include <fkit/Mutex.h>

using namespace fkit;

int main()
{
	try {
		File::open("non-existing");
		return 1;
	}
	catch (Exception &ex) {
		fout() << ex.what() << nl;
	}

	try {
		Mutex::create()->release();
		check(false);
	}
	catch (Exception &ex) {
		fout() << ex.what() << nl;
	}

	return 0;
}
