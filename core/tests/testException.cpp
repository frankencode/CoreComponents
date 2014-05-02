#include <flux/stdio.h>
#include <flux/check.h>
#include <flux/Mutex.h>
#include <flux/exceptions.h>

using namespace flux;

int main()
{
	try {
		Mutex::create()->release();
		check(false);
	}
	catch (Exception &ex) {
		fout() << ex.what() << nl;
	}

	try {
		String path = "test123.abc";
		if (!File::tryOpen(path)) FLUX_SYSTEM_RESOURCE_ERROR(errno, path);
	}
	catch (Exception &ex) {
		fout() << ex.what() << nl;
	}

	try {
		String path = "testabc.123";
		if (!File::tryOpen(path)) FLUX_SYSTEM_DEBUG_ERROR(errno);
	}
	catch (Exception &ex) {
		fout() << ex.what() << nl;
	}

	try {
		File::open("non-existing");
		return 1;
	}
	catch (Exception &ex) {
		fout() << ex.what() << nl;
	}

	return 0;
}
