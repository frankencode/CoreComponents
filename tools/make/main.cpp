/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/stdio>
#include <flux/exceptions>
#include "BuildPlan.h"

using namespace fluxmake;

int main(int argc, char **argv)
{
    String toolName = String(argv[0])->fileName();
    int exitCode = 0;
    try {
        exitCode = BuildPlan::create(argc, argv)->run();
    }
    catch (HelpError &) {
        fout(
            "Usage: %% [OPTION]... [DIR]\n"
            "Build binaries from source DIR.\n"
            "\n"
            "Options:\n"
            "  -debug           compile-in debug information\n"
            "  -release         release mode (NDEBUG defined)\n"
            "  -static          build static libraries\n"
            "  -verbose         verbose output\n"
            "  -configure       run include and link tests\n"
            "  -optimize        optimization level / strategy (0, 1, 2, 3, s, g)\n"
            "  -prefix          installation prefix (default: /usr)\n"
            "  -install         install applications, libraries and bundled files\n"
            "  -uninstall       delete installed files\n"
            "  -test            build all tests\n"
            "  -test-run        run all tests until first fails\n"
            "  -test-report     run all tests ($? = number of failed tests)\n"
            "  -test-args       list of arguments to pass to all tests\n"
            "  -compile-flags   custom compile flags\n"
            "  -link-flags      custom link flags\n"
            "  -compiler        select compiler\n"
            "  -concurrency     number of concurrent jobs to spawn\n"
            "  -simulate        print build commands without executing them\n"
            "  -blindfold       do not see any existing files\n"
            "  -bootstrap       write bootstrap script\n"
        ) << toolName;
    }
    catch (UsageError &ex) {
        ferr() << toolName << ": " << ex.message() << nl;
        return 1;
    }
    #ifdef NDEBUG
    catch (Exception &ex) {
        ferr() << toolName << ": " << ex.message() << nl;
        return 1;
    }
    #endif
    return exitCode;
}
