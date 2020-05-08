/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/testing/TestSuite>
#include <cc/stdio>
#include <cc/Variant>
#include <cc/System>
#include <cc/Map>
#include <cc/Random>

using namespace cc;
using namespace cc::testing;

namespace cc { template class Map<String, String>; }
namespace cc { template class Map<String, Variant>; }

int fib(int n)
{
    if (n == 0) return 0;
    if (n == 1) return 1;
    return fib(n - 1) + fib(n - 2);
}

class InsertionIteration: public TestCase
{
    void run()
    {
        typedef Map<String> StringMap;
        Ref<StringMap> names = StringMap::create();
        String test[6][2] = {
            { "Joe", "Doe" },
            { "Hans", "Mustermax" },
            { "Max", "Musterhans" },
            { "Otto", "Müller" },
            { "Johanna", "Berg" },
            { "Güther", "Becker" }
        };
        const int testCount = sizeof(test) / sizeof(test[0]);
        for (int i = 0; i < testCount; ++i)
            names->insert(test[i][0], test[i][1]);
        /*#ifndef NDEBUG
        CC_VERIFY(MapTester::test<StringMap>(names));
        #endif*/
        for (int i = 0; i < names->count(); ++i)
            fout("%% %%\n") << names->at(i)->key() << names->at(i)->value();
        for (int i = 0; i < testCount; ++i)
            CC_VERIFY(names->value(test[i][0]) == test[i][1]);
    }
};

class InsertionRemoval: public TestCase
{
    void run()
    {
        typedef Map<int> IntMap;
        Ref<IntMap> map = IntMap::create();
        const int n = 20;
        {
            Ref<Random> random = Random::open(0);
            for (int i = 0; i < n; ++i) {
                int key = random->get();
                int value = random->get();
                map->insert(key, value);
                /*#ifndef NDEBUG
                CC_VERIFY(MapTester::test<IntMap>(map));
                #endif*/
            }
        }
        {
            Ref<Random> random = Random::open(0);
            for (int i = 0; i < n; ++i) {
                int key = random->get();
                int value = random->get();
                CC_VERIFY(map->value(key) == value);
            }
        }
        {
            Ref<Random> random = Random::open(0);
            for (int i = 0; i < n; ++i) {
                int j = random->get(0, map->count() - 1);
                map->remove(map->at(j)->key());
                /*#ifndef NDEBUG
                CC_VERIFY(MapTester::test<IntMap>(map));
                #endif*/
            }
        }
    }
};

#if 0
class RangeSelection: public TestCase
{
    void run()
    {
        Ref< Map<int, int> > map = Map<int, int>::create();
        for (int i = 0; i < 20; ++i)
            map->insert(fib(i), i);
        for (int i = 0; i < map->count(); ++i)
            fout("map->at(%%) = %% (%%)\n") << i << map->at(i)->key() << map->at(i)->value();
        const int a = 20, b = 120;
        fout("In range [%%..%%]:\n") << a << b;
        int n = 0;
        for (int i = map->from(a), j = map->to(b); i <= j; ++i, ++n)
            fout("map->at(%%) = %% (%%)\n") << i << map->at(i)->key() << map->at(i)->value();
        CC_VERIFY(n == 4 && map->from(a) == 7 && map->to(b) == 10);
    }
};
#endif

#if 0
class SyntaxSugar: public TestCase
{
    void printMap(const Map<int> *map)
    {
        for (auto x: map)
            fout("%% => %%") << x->key() << x->value() << nl;
    }

    void run()
    {
        Ref< Map<int> > map = Map<int>::create();
        for (int i = 0; i < 20; ++i)
            map->insert(i, fib(i));
        printMap(map);
        fout() << "Keys:" << nl;
        for (auto key: map->getAllKeys())
            fout() << key << nl;
        fout() << "Values:" << nl;
        for (auto value: map->getAllValues())
            fout() << value << nl;
    }
};
#endif

int main(int argc, char **argv)
{
    CC_TESTSUITE_ADD(InsertionIteration);
    CC_TESTSUITE_ADD(InsertionRemoval);
    // CC_TESTSUITE_ADD(RangeSelection);
    // CC_TESTSUITE_ADD(SyntaxSugar);

    return TestSuite::instance()->run(argc, argv);
}
