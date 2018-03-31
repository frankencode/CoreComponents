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
#include <cc/Pile>

using namespace cc;
using namespace cc::testing;

class SimplePileTest: public TestCase
{
    void run() override
    {
        typedef Pile<String, int> TestPile;
        Ref<TestPile> layout = TestPile::create();
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
                layout->getItemExtentAt(i, &item, &extent);
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
                layout->getItemExtentAt(i, &item, &extent);
                fout() << item << " (" << extent << "): " << pos << ".." << pos + extent - 1 << nl;
                pos += extent;
            }
            fout() << nl;
        }
        {
            for (auto stop: layout->getInRange(3, 10)) {
                fout()
                    << stop->item() << " (" << stop->extent() << "): "
                    << stop->pos() << ".." << stop->pos() + stop->extent() - 1 << nl;
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

class IndexTrackingTest: public TestCase
{
    class TestItem: public Object, public IndexTracking
    {
    public:
        static Ref<TestItem> create(int value) { return new TestItem(value); }
        int value() const { return value_; }
    private:
        TestItem(int value): value_(value) {}
        int value_;
    };

    void run() override
    {
        typedef Pile< Ref<TestItem>, int > TestPile;
        Ref<TestPile> layout = TestPile::create();
        Ref<TestItem> item = TestItem::create(0);
        layout->insertAt(0, item, 1);
        for (int i = 1; i <= 10; ++i) {
            if (i % 2) layout->insertAt(layout->count(), TestItem::create(i), i);
            else layout->insertAt(0, TestItem::create(i), i);
            fout() << "item->getIndex() = " << item->getIndex() << nl;
            CC_VERIFY(item->getIndex() == i/2);
        }
    }
};

int main(int argc, char **argv)
{
    CC_TESTSUITE_ADD(SimplePileTest);
    CC_TESTSUITE_ADD(IndexTrackingTest);

    return TestSuite::instance()->run(argc, argv);
}
