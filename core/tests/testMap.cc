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
        auto names = Map<String>::create();
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
            fout("%% %%\n") << names->at(i)->key() << names->at(i)->value();
        for (int i = 0; i < testCount; ++i)
            CC_VERIFY(names->value(test[i][0]) == test[i][1]);
    }
};

class InsertionRemoval: public TestCase
{
    void run()
    {
        auto map = Map<int>::create();
        const int n = 20;
        {
            auto random = Random::open(0);
            for (int i = 0; i < n; ++i) {
                int key = random->get();
                int value = random->get();
                map->insert(key, value);
            }
        }
        {
            auto random = Random::open(0);
            for (int i = 0; i < n; ++i) {
                int key = random->get();
                int value = random->get();
                CC_VERIFY(map->value(key) == value);
            }
        }
        {
            auto random = Random::open(0);
            for (int i = 0; i < n; ++i) {
                int j = random->get(0, map->count() - 1);
                map->remove(map->at(j)->key());
            }
        }
    }
};

int main(int argc, char **argv)
{
    CC_TESTSUITE_ADD(InsertionIteration);
    CC_TESTSUITE_ADD(InsertionRemoval);

    return TestSuite::instance()->run(argc, argv);
}
