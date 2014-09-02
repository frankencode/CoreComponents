/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

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
	catch (std::exception &ex) {
		fout() << ex.what() << nl;
	}

	try {
		String path = "testabc.123";
		if (!File::tryOpen(path)) FLUX_SYSTEM_DEBUG_ERROR(errno);
	}
	catch (Exception &ex) {
		fout() << ex << nl;
	}

	try {
		File::open("non-existing");
		return 1;
	}
	catch (Exception &ex) {
		fout() << ex << nl;
	}

	return 0;
}
