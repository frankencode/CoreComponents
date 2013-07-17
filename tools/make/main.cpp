/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <fkit/stdio.h>
#include "BuildPlan.h"

using namespace fmake;

int main(int argc, char **argv)
{
	int exitCode = 0;
	try {
		exitCode = BuildPlan::create(argc, argv)->make();
	}
	catch (HelpException &) {
		fout(
			"Usage: %% [OPTION]... [DIR]\n"
			"Build binaries from source DIR.\n"
			"\n"
			"Options:\n"
			"  -debug      compile-in debug information\n"
			"  -release    release mode (NDEBUG defined)\n"
			"  -static     build static libraries\n"
			"  -simulate   print build commands without executing\n"
			"  -blindfold  do not see any existing files\n"
			"  -bootstrap  write bootstrap script\n"
			"  -test       build all tests\n"
			"  -test-run   run all tests\n"
			"  -verbose    verbose output\n"
			"  -optimize-speed  optimize for speed\n"
			"  -optimize-size   optimize for size\n"
			"  -optimize-debug  optimize without compromising debugging\n"
			"  -speed-optimization-level=<n>  speed optimization level\n"
			"  -size-optimization-level=<m>   size optimization level\n"
			"  -prefix     installation prefix (default: /usr)\n"
			"  -install    install headers and binaries\n"
			"  -uninstall  delete installed files\n"
		) << String(argv[0])->fileName();
	}
	catch (UserException &ex) {
		ferr("%%\n") << ex.message();
		exitCode = 7;
	}
	return exitCode;
}
