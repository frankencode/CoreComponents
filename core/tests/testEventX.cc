/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/testing/TestSuite>
#include <cc/stdio>
#include <cc/EventX>

using namespace cc;
using namespace cc::testing;

int main(int argc, char **argv)
{
    EventX trainArrived;
    trainArrived->connect([]{ fout() << "The train has arrived!" << nl; });
    trainArrived->connect([]{ fout() << "Caution!" << nl; });
    trainArrived->notify();
    fout() << sizeof(trainArrived) << nl;
    return 0;
}
