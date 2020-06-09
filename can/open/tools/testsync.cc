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
    auto can = CanSocket::open("can0");

    const double syncInterval = 0.06;
    const double syncTolerance = 0.1;

    const double syncIntervalMin = syncInterval - syncTolerance * syncInterval;
    const double syncIntervalMax = syncInterval + syncTolerance * syncInterval;

    double t0 = -1;
    double ta = t0;

    for (auto frame = CanFrame::create(); can->readFrame(frame);) {
        double t = System::now();
        if (t0 < 0) {
            t0 = t;
            ta = t;
        }
        fout() << fixed(t - t0, 3, 3) << ": " << frame;
        bool isSync = (frame->canId() == 0x80);
        if (isSync) {
            double dt = t - ta;
            ta = t;
            fout() << " (dt = " << fixed(dt, 3, 3) << ")" << nl;
            if (!(syncIntervalMin <= dt && dt <= syncIntervalMax) && ta != t0) fout() << "SYNC MISSING OR OUT OF TOLERANCE" << nl;
        }
        else
            fout() << nl;
    }

    return 0;
}
