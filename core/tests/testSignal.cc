/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/testing/TestSuite>
#include <cc/Signal>
#include <cc/stdio>

using namespace cc;
using namespace cc::testing;

int main(int argc, char **argv)
{
    Signal trainArrived;
    trainArrived >>[=]{ fout() << "The train has arrived!" << nl; };
    trainArrived >>[=]{ fout() << "Caution!" << nl; };
    trainArrived();
    fout() << sizeof(trainArrived) << nl;

    return 0;
}
