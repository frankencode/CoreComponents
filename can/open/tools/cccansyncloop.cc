/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/can/CanSocket>
#include <cc/System>
#include <cc/stdio>

using namespace cc;
using namespace cc::can;

int main(int argc, char **argv)
{
    CanSocket can0{"can0"};
    CanSocket can1{"can1"};

    CanFrame frameA;
    CanFrame frameB;

    frameA->setCanId(0x80);
    frameA->setPayloadCount(0);

    for (int i = 0; i < 1000; ++i) {
        auto tA = System::now();
        can0->writeFrame(frameA);
        can1->readFrame(&frameB);
        auto tB = System::now();
        fout() << fixed(tB - tA, 3, 6) << " s: " << frameB << nl;
    }

    return 0;
}
