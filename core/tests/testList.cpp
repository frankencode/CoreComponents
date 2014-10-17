/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/stdio>
#include <flux/check>
#include <flux/Random>
#include <flux/List>

using namespace flux;

template<class T>
void print(Ref< List<T> > list) {
    fout("[");
    for (int i = 0; i < list->count(); ++i) {
        fout() << list->at(i);
        if (i + 1 < list->count()) fout(", ");
    }
    fout("]\n");
}

int main()
{
    typedef List<int> IntList;

    {
        fout("Insertion, iteration...\n");
        Ref<IntList> list = IntList::create();
        for (int i = 0; i < 10; ++i)
            list->append(i);
        print(list);
        for (int i = 0; i < 10; ++i)
            check(list->at(i) == i);
    }
    {
        fout("Insertion, removal...\n");
        Ref<IntList> list = IntList::create();
        for (int i = 0; i < 10; ++i)
            list->append(i);
        print(list);
        for (int i = 0; i < list->count(); ++i)
            if (list->at(i) % 2 != 0) list->remove(i);
        print(list);
        for (int i = 0; i < 10; ++i) {
            if (i % 2 == 0)
                check(list->popFront() == i);
        }
    }
    {
        fout("Sorting, making unique...\n");
        Ref<IntList> list = IntList::create();
        Ref<Random> random = Random::open();
        for (int i = 0; i < 10; ++i)
            list->append(random->get(0, 99));
        print(list);
        Ref<IntList> list2 = list->sort();
        print(list2);
        check(list2->count() == list->count());
        for (int i = 0; i < list2->count() - 1; ++i)
            check(list2->at(i) <= list2->at(i + 1));
        Ref<IntList> list3 = list->unique();
        print(list3);
        for (int i = 0; i < list3->count() - 1; ++i)
            check(list3->at(i) < list3->at(i + 1));
    }
    {
        fout("Cloning...\n");
        Ref<Random> random = Random::open();
        Ref<IntList> a = IntList::create();
        for (int i = 0; i < 10; ++i)
            a->append(random->get(0, 99));
        Ref<IntList> b = IntList::clone(a);
        print(a);
        print(b);
        check(*a == *b);
    }
    {
        fout("Preallocation...\n");
        Ref<IntList> a = IntList::create(11);
        print(a);
        check(a->count() == 11);
    }
    return 0;
}
