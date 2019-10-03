/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/can/CanSocket>
#include <cc/Arguments>
#include <cc/System>
#include <cc/Thread>
#include <cc/stdio>

using namespace cc;
using namespace cc::can;

void runTest(const VariantMap *options)
{
    bool transmit = options->value("transmit");
    bool receive  = options->value("receive");
    String interface = options->value("interface");
    canid_t cobId = int(options->value("cob-id"));
    canid_t invCobId = int(options->value("cob-id-inv"));
    double refreshTime = options->value("refresh-time");
    double cycleTime = options->value("cycle-time");
    double validationTime = options->value("validation-time");

    auto socket = CanSocket::open(interface);

    if (transmit) {
        auto frame = CanFrame::create();
        auto invFrame = CanFrame::create();
        frame->setCanId(cobId);
        invFrame->setCanId(invCobId);
        uint8_t r = 1;
        while (true) {
            double t = System::now();
            for (int i = 0; i < 8; ++i) {
                frame->payloadAt(i) = r;
                invFrame->payloadAt(i) = ~r;
                ++r;
            }
            socket->writeFrame(frame);
            socket->writeFrame(invFrame);
            Thread::sleepUntil(t + refreshTime);
        }
    }
    else if (receive) {
        auto frame = CanFrame::create();
        auto invFrame = CanFrame::create();
        double t0 = -1;
        while (true) {
            if (!socket->readFrame(frame)) break;
            if (frame->canId() == invCobId) continue;
            double tv = System::now() + validationTime;
            fout() << frame << nl;
            while (true) {
                double dt = tv - System::now();
                bool timedOut = (dt <= 0);
                if (!timedOut) timedOut = !socket->waitFrame(1000 * dt);
                if (timedOut) {
                    ferr() << "ERROR: SRDO VALIDATION TIMED OUT" << nl;
                    break;
                }
                socket->readFrame(invFrame);
                if (invFrame->canId() == cobId) {
                    ferr() << "ERROR: INVERSE SRDO MISSING" << nl;
                    break;
                }
                if (invFrame->canId() == invCobId) break;
            }
            {
                bool ok = true;
                for (int i = 0; i < frame->payloadCount(); ++i) {
                    if (frame->payloadAt(i) != uint8_t(~invFrame->payloadAt(i))) {
                        ok = false;
                        break;
                    }
                }
                if (!ok) {
                    ferr() << "ERROR: SRDO VALIDATION FAILED" << nl;
                    continue;
                }
            }
            double t1 = System::now();
            if (t0 > 0) {
                double dt = t1 - t0;
                if (dt > cycleTime) {
                    ferr() << "ERROR: CYCLE TIME EXCEEDED" << nl;
                    t0 = t1;
                    continue;
                }
            }
            t0 = t1;
            fout() << "OK" << nl;
        }
    }
    else
        throw HelpRequest{};
}

int main(int argc, char **argv)
{
    String toolName = String(argv[0])->fileName();

    try {
        auto arguments = Arguments::parse(argc, argv);

        auto options = VariantMap::create();
        {
            options->insert("transmit", false);
            options->insert("receive", false);
            options->insert("interface", "can0");
            options->insert("cob-id", 0x101);
            options->insert("cob-id-inv", 0x102);
            options->insert("refresh-time", 1.);
            options->insert("cycle-time", 1.1);
            options->insert("validation-time", 0.1);
            arguments->validate(options);
            arguments->override(options);
        }

        runTest(options);
    }
    catch (HelpRequest &) {
        fout(
            "Usage: %% [OPTION]... [DIR]...\n"
            "Test transmission/reception of a SRDO.\n"
            "\n"
            "Options:\n"
            "  -transmit         emit random SRDOs\n"
            "  -receive          receive SRDOs and validate timings\n"
            "  -interface        CAN interface to operate on (defaults to \"can0\")\n"
            "  -cob-id           SRDO COB ID (defaults to 0x101)\n"
            "  -cob-id-inv       inverse SRDO COB ID (defaults to 0x102)\n"
            "  -refresh-time     time interval in which new SRDOs are generated\n"
            "  -cycle-time       maximum allowed time between reception of two inverse SRDOs (aka SCT)\n"
            "  -validation-time  maximum allowed time between reception of a SRDO and its corresponding reverse SRDO (aka SRVT)\n"
        ) << toolName;

        return 1;
    }
    catch (Exception &ex) {
        ferr() << toolName << ": " << ex << nl;
        return 1;
    }

    return 0;
}
