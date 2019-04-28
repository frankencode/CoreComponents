/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/stdio>
#include <cc/exceptions>
#include "NodeMaster.h"

using namespace ccnode;

int main(int argc, char **argv)
{
    String toolName = String{argv[0]}->fileName();
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
            "  -port      port to listen on (default 8080)\n"
            "  -protocol  protocol: IPv4 or IPv6 (default both)\n"
            "  -user      switch to user after opening listening socket\n"
            "  -daemon    start as a daemon\n"
        ) << toolName;
    }
    // #ifdef NDEBUG
    catch (Exception &ex) {
        ferr() << ex.message() << nl;
        exitCode = 7;
    }
    // #endif
    return exitCode;
}
