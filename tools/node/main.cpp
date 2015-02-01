/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/stdio>
#include <flux/exceptions>
#include "NodeMaster.h"

using namespace fluxnode;

int main(int argc, char **argv)
{
    String toolName = String(argv[0])->fileName();
    int exitCode = 0;
    try {
        exitCode = NodeMaster::run(argc, argv);
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
    #ifdef NDEBUG
    catch (Exception &ex) {
        ferr() << toolName << ": " << ex.message() << nl;
        exitCode = 7;
    }
    #endif
    return exitCode;
}
