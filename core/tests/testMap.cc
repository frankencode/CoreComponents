/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/testing/TestSuite>
#include <cc/stdio>
#include <cc/System>
#include <cc/Map>
#if 0
#include <cc/Random>
#include <map>
#endif

using namespace cc;
using namespace cc::testing;

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
        for (int i = 0; i < names->count(); ++i)
            fout("%% %%\n") << names->keyAt(i) << names->valueAt(i);
        for (int i = 0; i < testCount; ++i)
            CC_VERIFY(names->value(test[i][0]) == test[i][1]);
    }
};

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
        for (auto key: map->keys())
            fout() << key << nl;
        fout() << "Values:" << nl;
        for (auto value: map->values())
            fout() << value << nl;
    }
};

int main(int argc, char **argv)
{
    CC_TESTSUITE_ADD(InsertionIteration);
    CC_TESTSUITE_ADD(RangeSelection);
    CC_TESTSUITE_ADD(SyntaxSugar);

    return TestSuite::instance()->run(argc, argv);
}

#if 0
{
    fout() << "Performance..." << nl;
    const int n = 100000;
    {
        std::map<int, int> map;
        double t0 = System::now();
        for (int i = 0; i < n; ++i)
            map[i] = i;
        fout("std::map, %% insertions: dt = %% us\n") << n << int((System::now() - t0) * 1e6);
        t0 = System::now();
        int s = 0;
        for (std::map<int, int>::const_iterator i = map.begin(); i != map.end(); ++i)
            s += i->second;
        fout("std::map, %% iteration steps: dt = %% us\n") << n << int((System::now() - t0) * 1e6);
    }
    {
        Ref< Map<int, int> > map = Map<int, int>::create();
        double t0 = System::now();
        for (int i = 0; i < n; ++i)
            map->establish(i, i);
        fout("cc::Map, %% insertions: dt = %% us\n") << n << int((System::now() - t0) * 1e6);
        t0 = System::now();
        int s = 0;
        for (int i = 0; i < n; ++i)
            s += map->valueAt(i);
        fout("cc::Map, %% iteration steps: dt = %% us\n") << n << int((System::now() - t0) * 1e6);
    }
}
#endif
