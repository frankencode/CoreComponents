/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/testing/TestSuite>
#include <cc/stdio>
#include <cc/debug>
#include <cc/Random>
#include <cc/Layout>

using namespace cc;
using namespace cc::testing;

class SimpleLayout: public TestCase
{
    void run() override
    {
        typedef Layout<String, int> TestLayout;
        Ref<TestLayout> layout = TestLayout::create();
        layout->append("Item 1", 2);
        layout->append("Item 2", 3);
        layout->append("Item 3", 1);
        layout->append("Item 4", 4);
        CC_VERIFY(layout->count() == 4);
        for (int i = 0; i < layout->count(); ++i) {
            String item;
            int extent = 0;
            int pos = layout->getPosAt(i);
            layout->getItemAt(i, &item, &extent);
            fout() << item << " (" << extent << "): " << pos << nl;
        }
    }
};

int main(int argc, char **argv)
{
    CC_TESTSUITE_ADD(SimpleLayout);

    return TestSuite::instance()->run(argc, argv);
}
