/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/testing/TestSuite>
#include <cc/stdio>
#include <cc/variant>

using namespace cc;
using namespace cc::testing;

class Comparisms: public TestCase
{
    void run()
    {
        variant x = 1;
        variant y = "2.";
        variant z = true;
        variant a = string{"abc"};

        CC_VERIFY(!(x < y));
        CC_VERIFY(!(x == y));
        CC_VERIFY(!(x < z));
        CC_VERIFY(x == z);
        CC_VERIFY(y < a);
        CC_VERIFY(y != a);
        CC_VERIFY(!(variant() < x));
        CC_VERIFY(!(x == variant()));
        CC_VERIFY(!(variant() < variant()));
        CC_VERIFY(variant() == variant());
    }
};

class Casting: public TestCase
{
    void run()
    {
        variant value = StringList::create() << "A" << "B" << "C";
        const StringList *list = variant::cast<const StringList *>(value);
        fout() << list->join(", ") << nl;
        CC_VERIFY(list->join() == "ABC");
    }
};

int main(int argc, char **argv)
{
    CC_TESTSUITE_ADD(Comparisms);
    CC_TESTSUITE_ADD(Casting);

    return TestSuite::instance()->run(argc, argv);
}
