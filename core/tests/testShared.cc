/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/testing/TestSuite>
#include <cc/Shared>
#include <cc/DEBUG>
#include <cc/stdio>

using namespace cc;
using namespace cc::testing;

class Monster
{
public:
    Monster(const String &species):
        me{species}
    {}

    String species() const { return me().species; }
    int health() const { return me().health; }

protected:
    struct State {
        String species;
        int health { 100 };
    };

    Shared<State> me;
};

int main(int argc, char **argv)
{
    Monster a{"A"};
    Monster b{"B"};
    Monster c = a;
    CC_INSPECT(a.species());
    CC_INSPECT(b.species());
    CC_INSPECT(c.species());
    CC_INSPECT(a.health());
    return 0;
}
