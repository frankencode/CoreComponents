/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <flux/stdio.h>
#include <flux/errors.h>
#include "NodeMaster.h"

using namespace fluxnode;

int main(int argc, char **argv)
{
	String toolName = String(argv[0])->fileName();
	int exitCode = 0;
	try {
		exitCode = nodeMaster()->run(argc, argv);
	}
	catch (HelpError &) {
		fout(
			"Usage: %% [OPTION]... [CONFIG]|[DIR]...\n"
			"Start a HTTP server on this machine.\n"
			"\n"
			"Options:\n"
			"  -address   host name or network address to listen on\n"
			"  -port      port to listen on (default 8000)\n"
			"  -protocol  protocol: IPv4 or IPv6 (default both)\n"
			"  -user      switch to user after opening listening socket\n"
			"  -daemon    start as a daemon\n"
		) << toolName;
	}
	catch (Error &ex) {
		ferr() << toolName << ": " << ex.message() << nl;
		exitCode = 7;
	}
	return exitCode;
}
