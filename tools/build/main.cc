/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/build/BuildPlan>
#include <cc/Bundle>
#include <cc/exceptions>
#include <cc/stdio>

int main(int argc, char *argv[])
{
    using namespace cc::build;
    using namespace cc;

    String toolName = String{argv[0]}.fileName();
    int exitCode = 0;
    try {
        exitCode = BuildPlan{argc, argv}.run();
    }
    catch (HelpRequest &) {
        fout(
            "Usage: %% [OPTION]... [DIR]\n"
            "Build binaries from source DIR.\n"
            "\n"
            "Options:\n"
            "  -debug           compile-in debug information\n"
            "  -release         release mode (NDEBUG defined)\n"
            "  -static          build static libraries\n"
            "  -verbose         verbose output\n"
            "  -optimize        optimization level / strategy (0, 1, 2, 3, 4, s, g)\n"
            "  -root            file system root for installation (default: \"/\")\n"
            "  -prefix          installation prefix (default: '/usr/local')\n"
            "  -setup           setup convenience Makefile and scripts\n"
            "  -insight         generate compile_commands.json files (e.g. for clangd)\n"
            "  -configure       configure the dependencies and show results\n"
            "  -configure-list  list all system dependencies\n"
            "  -prepare         evaluate predicate rules\n"
            "  -preprocess      print preprocessed sources to stdout\n"
            "  -clean           remove all files generated by the build process\n"
            "  -install         install applications, libraries and bundled files\n"
            "  -uninstall       delete installed files\n"
            "  -test            build all tests\n"
            "  -test-run        run all tests until first fails\n"
            "  -test-run-jobs   number of concurrent jobs to spawn for running tests\n"
            "  -test-report     run all tests ($? = number of failed tests)\n"
            "  -test-args       list of arguments to pass to all tests\n"
            "  -compiler        select compiler\n"
            "  -jobs            number of concurrent jobs to spawn\n"
            "  -simulate        print build commands without executing them\n"
            "  -blindfold       do not see any existing files\n"
            "  -bootstrap       write bootstrap script\n"
            "  -query           query given properties (e.g. 'name' or 'version')\n"
            "  -query-all       query all properties\n"
            "  -pkg-config      print package configuration (*.pc)\n"
            "  -version         print %% version\n"
        ) << toolName;
    }
    catch (VersionRequest &ex) {
        fout() << "v" << CC_BUNDLE_VERSION << nl;
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