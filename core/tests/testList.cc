/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/testing/TestSuite>
#include <cc/stdio>
#include <cc/Random>
#include <cc/List>

using namespace cc;
using namespace cc::testing;

template<class T>
void print(Ref< List<T> > list) {
    fout("[");
    for (int i = 0; i < list->count(); ++i) {
        fout() << list->at(i);
        if (i + 1 < list->count()) fout(", ");
    }
    fout("]\n");
}

typedef List<int> IntList;

class InsertionIteration: public TestCase
{
    void run() {
        Ref<IntList> list = IntList::create();
        for (int i = 0; i < 10; ++i)
            list->append(i);
        print(list);
        for (int i = 0; i < 10; ++i)
            CC_VERIFY(list->at(i) == i);
    }
};

class InsertionRemoval: public TestCase
{
    void run() {
        Ref<IntList> list = IntList::create();
        for (int i = 0; i < 10; ++i)
            list->append(i);
        print(list);
        for (int i = 0; i < list->count(); ++i)
            if (list->at(i) % 2 != 0) list->removeAt(i);
        print(list);
        for (int i = 0; i < 10; ++i) {
            if (i % 2 == 0)
                CC_VERIFY(list->popFront() == i);
        }
    }
};

class Sorting: public TestCase
{
    void run() {
        Ref<IntList> list = IntList::create();
        Ref<Random> random = Random::open();
        for (int i = 0; i < 10; ++i)
            list->append(random->get(0, 99));
        print(list);
        Ref<IntList> list2 = list->sort();
        print(list2);
        CC_VERIFY(list2->count() == list->count());
        for (int i = 0; i < list2->count() - 1; ++i)
            CC_VERIFY(list2->at(i) <= list2->at(i + 1));
        /*Ref<IntList> list3 = list->unique();
        print(list3);
        for (int i = 0; i < list3->count() - 1; ++i)
            CC_VERIFY(list3->at(i) < list3->at(i + 1));*/
    }
};

class Cloning: public TestCase
{
    void run() {
        Ref<Random> random = Random::open();
        Ref<IntList> a = IntList::create();
        for (int i = 0; i < 10; ++i)
            a->append(random->get(0, 99));
        Ref<IntList> b = IntList::copy(a);
        print(a);
        print(b);
        CC_VERIFY(*a == *b);
    }
};

class Preallocation: public TestCase
{
    void run() {
        Ref<IntList> a = IntList::create(11);
        print(a);
        CC_VERIFY(a->count() == 11);
    }
};

class SyntaxSugar: public TestCase
{
    void printList(const IntList *l) {
        for (auto x: l)
            fout() << x << nl;
    }

    void run() {
        for (auto x: IntList::create() << 1 << 2 << 3)
            fout() << x << nl;
        printList(IntList::create() << 1 << 2 << 3);
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

    void run() {
        typedef List< Ref<TestItem> > TestList;
        Ref<TestList> list = TestList::create();
        Ref<TestItem> item = TestItem::create(0);
        list << item;
        for (int i = 1; i <= 10; ++i) {
            if (i % 2) list->pushBack(TestItem::create(i));
            else list->pushFront(TestItem::create(i));
            fout() << "item->getIndex() = " << item->getIndex() << nl;
            CC_VERIFY(item->getIndex() == i/2);
        }
    }
};

int main(int argc, char **argv)
{
    CC_TESTSUITE_ADD(InsertionIteration);
    CC_TESTSUITE_ADD(InsertionRemoval);
    CC_TESTSUITE_ADD(Sorting);
    CC_TESTSUITE_ADD(Cloning);
    CC_TESTSUITE_ADD(Preallocation);
    CC_TESTSUITE_ADD(SyntaxSugar);
    CC_TESTSUITE_ADD(IndexTrackingTest);

    return TestSuite::instance()->run(argc, argv);
}
