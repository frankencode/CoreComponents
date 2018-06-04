/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/testing/TestSuite>
#include <cc/stdio>
#include <cc/debug>
#include <cc/ui/Item>

using namespace cc;
using namespace cc::ui;
using namespace cc::testing;

class TestVisual;

class TestItem: public Item
{
public:
    static Ref<TestItem> create(String name) { return new TestItem(name); }

    String toString() const
    {
        Ref<StringList> l = StringList::create();
        for (const Item *item = this; item; item = item->parent()) {
            l->pushFront(static_cast<const TestItem *>(item)->name_);
        }
        return l->join("/");
    }

private:
    virtual Ref<ItemVisual> createVisual(ItemLayout *layout) override;

    TestItem(String name): name_(name) {}
    String name_;
};

class TestVisual: public ItemVisual {
public:
    static Ref<TestVisual> create(TestItem *item, const ItemLayout *layout)
    {
        return new TestVisual(item, layout);
    }

    void paint(Painter &) override {}

private:

    const TestItem *testItem() const { return static_cast<TestItem *>(item()); }

    void update() override { size = Size{1., double(testItem()->toString()->count())}; }

    TestVisual(TestItem *item, const ItemLayout *layout):
        ItemVisual(item, layout)
    {}
};

Ref<ItemVisual> TestItem::createVisual(ItemLayout *layout)
{
    return TestVisual::create(this, layout);
}

class SimpleVisualLayoutTest: public TestCase
{
public:
    void run() override
    {
        Ref<TestItem> root = TestItem::create("");

        Ref<TestItem> usr = TestItem::create("usr");
        Ref<TestItem> usrLib = TestItem::create("lib");
        usrLib->append(TestItem::create("libc.so"));
        {
            usr->append(TestItem::create("bin"));
            usr->append(usrLib);
            usr->append(TestItem::create("share"));
            usr->append(TestItem::create("src"));
            usr->append(TestItem::create("local"));
        }
        Ref<TestItem> lib = TestItem::create("lib");
        root->append(lib);
        root->append(TestItem::create("mnt"));
        root->append(TestItem::create("var"));
        Ref<ItemLayout> layout = ItemLayout::create(root);

        auto printLayout = [&]{
            for (int j = 0; j < layout->count(); ++j)
                fout() << j << ": " << layout->getPosAt(j) << ": " << static_cast<TestItem *>(layout->at(j)->item())->toString() << nl;
        };
        auto printExtents = [&]{
            CC_INSPECT(root->totalCount());
            for (int i = 0; i < root->count(); ++i)
                fout() << "totalCount(" << i << ") = " << root->at(i)->totalCount() << nl;
        };
        auto printDebug = [&]{
            fout() << "----" << nl;
            printExtents();
            Ref<StringList> l = StringList::create();
            for (int x: root->delta()->indexPath()) l << str(x);
            fout() << "[" << l->join(", ") << "]" << nl;
            printLayout();
        };
        layout->changed->connect(printDebug);
        printDebug();
        root->append(usr);
        lib->append(TestItem::create("modules"));

        CC_VERIFY(root->count() == 4);
        CC_VERIFY(root->totalCount() == 11);
        CC_VERIFY(layout->getPosAt(root->totalCount() - 1) == 78);
    }
};

int main(int argc, char **argv)
{
    CC_TESTSUITE_ADD(SimpleVisualLayoutTest);

    return TestSuite::instance()->run(argc, argv);
}
