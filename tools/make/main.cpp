/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <flux/stdio.h>
#include "BuildPlan.h"

using namespace fmake;

int main(int argc, char **argv)
{
	int exitCode = 0;
	try {
		exitCode = BuildPlan::create(argc, argv)->run();
	}
	catch (HelpException &) {
		ferr(
			"Usage: %% [OPTION]... [DIR]\n"
			"Build binaries from source DIR.\n"
			"\n"
			"Options:\n"
			"  -debug           compile-in debug information\n"
			"  -release         release mode (NDEBUG defined)\n"
			"  -static          build static libraries\n"
			"  -verbose         verbose output\n"
			"  -optimize-speed  optimize for speed\n"
			"  -optimize-size   optimize for size\n"
			"  -optimize-debug  optimize without compromising debugging\n"
			"  -prefix          installation prefix (default: /usr)\n"
			"  -install         install headers and binaries\n"
			"  -uninstall       delete installed files\n"
			"  -test            build all tests\n"
			"  -test-run        run all tests\n"
			"  -compile-flags   custom compile flags\n"
			"  -link-flags      custom link flags\n"
			"  -compiler        select compiler\n"
			"  -concurrency     number of concurrent jobs to spawn\n"
			"  -simulate        print build commands without executing them\n"
			"  -blindfold       do not see any existing files\n"
			"  -bootstrap       write bootstrap script\n"
		) << String(argv[0])->fileName();
	}
	catch (UserException &ex) {
		fout("%%\n") << ex.message();
		exitCode = 7;
	}
	return exitCode;
}
