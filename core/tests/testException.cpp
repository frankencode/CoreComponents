#include <flux/stdio.h>
#include <flux/check.h>
#include <flux/Mutex.h>

using namespace flux;

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
