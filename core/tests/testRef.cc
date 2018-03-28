/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/testing/TestSuite>
#include <cc/stdio>

using namespace cc;
using namespace cc::testing;

class ComparismsTest: public TestCase
{
    void run()
    {
        class A: public Object { friend class Object; };
        Ref<A> a = Object::create<A>();
        Ref<A> b = Object::create<A>();
        CC_VERIFY(a == a);
        CC_VERIFY(a != b);
    }
};

int main(int argc, char **argv)
{
    CC_TESTSUITE_ADD(ComparismsTest);

    return TestSuite::instance()->run(argc, argv);
}
