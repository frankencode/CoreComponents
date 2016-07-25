/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/testing/TestSuite>
#include <cc/stdio>
#include <cc/Version>

using namespace cc;
using namespace cc::testing;

class Comparisms: public TestCase
{
    void run()
    {
        Version a("1.2.3");
        Version b(1, 2, 3);
        Version c(1, 2);

        fout("a = %%\n") << a;
        fout("b = %%\n") << b;
        fout("c = %%\n") << c;

        CC_VERIFY(a == b);
        CC_VERIFY(str(c) == "1.2.0");
        CC_VERIFY(c < a);
    }
};

int main(int argc, char **argv)
{
    CC_TESTSUITE_ADD(Comparisms);

    return TestSuite::instance()->run(argc, argv);
}
