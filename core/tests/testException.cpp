/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/testing/TestSuite>
#include <flux/stdio>
#include <flux/File>
#include <flux/Mutex>
#include <flux/exceptions>

using namespace flux;
using namespace flux::testing;

class ThreadingExceptions: public TestCase
{
    void run()
    {
        try {
            Mutex::create()->release();
            FLUX_VERIFY(false);
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
            if (!File::tryOpen(path)) FLUX_SYSTEM_DEBUG_ERROR(errno);
            FLUX_VERIFY(false);
        }
        catch (Exception &ex) {
            fout() << ex << nl;
        }

        try {
            File::open("non-existing");
            FLUX_VERIFY(false);
        }
        catch (Exception &ex) {
            fout() << ex << nl;
        }
    }
};

int main(int argc, char **argv)
{
    #ifndef NDEBUG
    FLUX_TESTSUITE_ADD(ThreadingExceptions);
    #endif
    FLUX_TESTSUITE_ADD(FileExceptions);

    return testSuite()->run(argc, argv);
}
