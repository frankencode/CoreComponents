/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/testing/TestSuite>
#include <cc/stdio>
#include <cc/Variant>

using namespace cc;
using namespace cc::testing;

class Comparisms: public TestCase
{
    void run()
    {
        Variant x = 1;
        Variant y = "2.";
        Variant z = true;
        Variant a = String("abc");

        CC_VERIFY(!(x < y));
        CC_VERIFY(!(x == y));
        CC_VERIFY(!(x < z));
        CC_VERIFY(x == z);
        CC_VERIFY(y < a);
        CC_VERIFY(y != a);
        CC_VERIFY(!(Variant() < x));
        CC_VERIFY(!(x == Variant()));
        CC_VERIFY(!(Variant() < Variant()));
        CC_VERIFY(Variant() == Variant());
    }
};

int main(int argc, char **argv)
{
    CC_TESTSUITE_ADD(Comparisms);

    return TestSuite::instance()->run(argc, argv);
}
