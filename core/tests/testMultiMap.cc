/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/testing/TestSuite>
#include <cc/stdio>
#include <cc/MultiMap>
#include <cc/Random>

using namespace cc;
using namespace cc::testing;

namespace cc { template class MultiMap<string>; }

class InsertionIteration: public TestCase
{
    void run()
    {
        auto random = Random::open(0);
        const int n = 40;
        const int a = 0, b = 10;

        auto map = MultiMap<int>::create();
        for (int i = 0; i < n; ++i) {
            int key = random->get(a, b);
            map->insert(key, i);
        }

        {
            int x2 = 0;
            for (int i = 0; i < map->count(); ++i) {
                int x = map->at(i)->key();
                int y = map->at(i)->value();
                fout("%%: %% => %%\n") << i << x << y;
                if (i > 0) CC_VERIFY(x2 <= x);
                x2 = x;
            }
        }

        for (int x = a; x < b; ++x) {
            fout("%% => ") << x;
            MultiMap<int>::const_iterator t;
            if (map->find<FindFirst>(x, &t)) {
                int y2 = 0; int i = 0;
                while (t && t->key() == x) {
                    int y = t->value();
                    fout() << y << " ";
                    if (i > 0) CC_VERIFY(y2 <= y);
                    y2 = y;
                    ++t;
                    ++i;
                }
            }
            fout() << nl;
        }

        for (int x = a; x < b; ++x) {
            fout("%% => ") << x;
            MultiMap<int>::const_iterator t;
            if (map->find<FindLast>(x, &t)) {
                int y2 = 0; int i = 0;
                while (t && t->key() == x) {
                    int y = t->value();
                    fout() << y << " ";
                    if (i > 0) CC_VERIFY(y2 >= y);
                    y2 = y;
                    --t;
                    ++i;
                }
            }
            fout() << nl;
        }
    }
};

int main(int argc, char **argv)
{
    CC_TESTSUITE_ADD(InsertionIteration);

    return TestSuite::instance()->run(argc, argv);
}
