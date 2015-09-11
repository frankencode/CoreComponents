/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/testing/TestSuite>
#include <flux/stdio>
#include <flux/System>
#include <flux/Map>
#if 0
#include <flux/Random>
#include <map>
#endif

using namespace flux;
using namespace flux::testing;

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
        typedef Map<String, String> StringMap;
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
            FLUX_VERIFY(names->value(test[i][0]) == test[i][1]);
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
        for (int i = map->first(a), j = map->last(b); i <= j; ++i, ++n)
            fout("map->at(%%) = %% (%%)\n") << i << map->at(i)->key() << map->at(i)->value();
        FLUX_VERIFY(n == 4 && map->first(a) == 7 && map->last(b) == 10);
    }
};

int main(int argc, char **argv)
{
    FLUX_TESTSUITE_ADD(InsertionIteration);
    FLUX_TESTSUITE_ADD(RangeSelection);

    return testSuite()->run(argc, argv);
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
        fout("flux::Map, %% insertions: dt = %% us\n") << n << int((System::now() - t0) * 1e6);
        t0 = System::now();
        int s = 0;
        for (int i = 0; i < n; ++i)
            s += map->valueAt(i);
        fout("flux::Map, %% iteration steps: dt = %% us\n") << n << int((System::now() - t0) * 1e6);
    }
}
#endif
