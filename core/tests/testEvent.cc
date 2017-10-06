/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/testing/TestSuite>
#include <cc/stdio>
#include <cc/Event>

using namespace cc;
using namespace cc::testing;

int main(int argc, char **argv)
{
    Event trainArrived;
    trainArrived->listen([]{ fout() << "The train has arrived!" << nl; });
    trainArrived->listen([]{ fout() << "Caution!" << nl; });
    trainArrived->notify();
    fout() << sizeof(trainArrived) << nl;

    return 0;
}
