/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/testing/TestSuite>
#include <cc/stdio>
#include <cc/Dir>

using namespace cc;
using namespace cc::testing;

class ListCwd: public TestCase
{
    void run() {
        for (auto path: Dir::open("."))
            fout() << path << nl;
    }
};

int main(int argc, char **argv)
{
    CC_TESTSUITE_ADD(ListCwd);
    return TestSuite::instance()->run(argc, argv);
}
