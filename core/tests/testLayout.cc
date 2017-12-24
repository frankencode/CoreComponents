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
        layout->append("Item 5", 1);
        CC_VERIFY(layout->count() == 5);
        {
            for (int i = 0; i < layout->count(); ++i) {
                String item;
                int extent = 0;
                int pos = layout->getPosAt(i);
                layout->getItemAt(i, &item, &extent);
                fout() << item << " (" << extent << "): " << pos << ".." << pos + extent - 1  << nl;
            }
            fout() << nl;
        }
        {
            int i0 = 0, i1 = 0;
            int pos = 0;
            layout->getView(3, 10, &i0, &i1, &pos);
            CC_ASSERT(i0 == 1 && i1 == 4);
            for (int i = i0; i < i1; ++i) {
                String item;
                int extent = 0;
                layout->getItemAt(i, &item, &extent);
                fout() << item << " (" << extent << "): " << pos  << ".." << pos + extent - 1 << nl;
                pos += extent;
            }
            fout() << nl;
        }
        {
            for (auto item: layout)
                fout() << item << nl;
            fout() << nl;
        }
    }
};

int main(int argc, char **argv)
{
    CC_TESTSUITE_ADD(SimpleLayout);

    return TestSuite::instance()->run(argc, argv);
}
