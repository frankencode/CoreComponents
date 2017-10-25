/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/testing/TestSuite>
#include <cc/debug>
#include <cc/Vector>

using namespace cc;
using namespace cc::testing;

class SimpleUsage: public TestCase
{
    void run()
    {
        Point a { 1, 2 };
        Point b { 3, 4 };
        Point c = a + b;
        CC_INSPECT(c);
        CC_VERIFY(c[0] == 4 && c[1] == 6);
    }
};

int main(int argc, char **argv)
{
    CC_TESTSUITE_ADD(SimpleUsage);

    return TestSuite::instance()->run(argc, argv);
}
