/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/testing/TestSuite>
#include <cc/stdio>
#include <cc/File>
#include <cc/Mutex>
#include <cc/exceptions>

using namespace cc;
using namespace cc::testing;

class ThreadingExceptions: public TestCase
{
    void run()
    {
        try {
            Mutex::create()->release();
            CC_VERIFY(false);
        }
        catch (Exception &ex) {
            fout() << ex << nl;
        }
    }
};

class FileExceptions: public TestCase
{
    void run()
    {
        try {
            String path = "testabc.123";
            if (!File::tryOpen(path)) CC_SYSTEM_DEBUG_ERROR(errno);
            CC_VERIFY(false);
        }
        catch (Exception &ex) {
            fout() << ex << nl;
        }

        try {
            File::open("non-existing");
            CC_VERIFY(false);
        }
        catch (Exception &ex) {
            fout() << ex << nl;
        }
    }
};

int main(int argc, char **argv)
{
    #ifndef NDEBUG
    CC_TESTSUITE_ADD(ThreadingExceptions);
    #endif
    CC_TESTSUITE_ADD(FileExceptions);

    return TestSuite::instance()->run(argc, argv);
}
