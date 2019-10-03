/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/can/CanSocket>
#include <cc/Arguments>
#include <cc/stdio>
#include <cc/System>
#include <cc/Map>
#include <cc/exceptions>
#include <cc/debug>
#include <limits>

using namespace cc;
using namespace cc::can;

void safetyListen(String interface, int canId, int invCanId, double cycleTime, double validationTime, double duration);

int main(int argc, char **argv)
{
    String toolName = String(argv[0])->fileName();

    try {
        Ref<Arguments> arguments = Arguments::parse(argc, argv);

        Ref<VariantMap> options = VariantMap::create();
        {
            options->insert("interface", "can0");
            options->insert("can-id", 0);
            options->insert("inv-can-id", 0);
            options->insert("cycle-time", 0.);
            options->insert("validation-time", 0.);
            options->insert("duration", 0.);
            arguments->validate(options);
            arguments->override(options);
        }

        if (arguments->items()->count() > 0) throw HelpRequest{};

        String interface = options->value("interface");
        int canId = options->value("can-id");
        int invCanId = options->value("inv-can-id");
        double cycleTime = double(options->value("cycle-time"));
        double validationTime = double(options->value("validation-time"));
        double duration = double(options->value("duration"));

        if (canId <= 0) {
            ferr() << "Please provide a CAN ID ('-can-id=<number>')" << nl;
            throw HelpRequest{};
        }
        if (cycleTime <= 0) {
            ferr() << "Please provide a cycle time ('-cycle-time=<ms>')" << nl;
            throw HelpRequest{};
        }
        if (validationTime <= 0) {
            ferr() << "Please provide a validation time ('-validation-time=<ms>')" << nl;
            throw HelpRequest{};
        }

        if (invCanId <= 0) invCanId = canId + 1;

        CC_INSPECT(interface);
        CC_INSPECT(hex(canId));
        CC_INSPECT(hex(invCanId));
        CC_INSPECT(fixed(cycleTime, 3, 3));
        CC_INSPECT(fixed(validationTime, 3, 3));
        CC_INSPECT(duration);

        safetyListen(interface, canId, invCanId, cycleTime, validationTime, duration);
    }
    catch (HelpRequest &) {
        fout(
            "Usage: %% [OPTION]... [DIR]...\n"
            "Listen to the CAN bus and report about a certain SRDO.\n"
            "\n"
            "Options:\n"
            "  -interface=<string>           CAN interface to listen on (\"can0\" by default)\n"
            "  -can-id=<number>              CAN ID of the SRDO\n"
            "  -inv-can-id=<number>          CAN ID of the inverse SRDO (defaults to CAN ID + 1)\n"
            "  -cycle-time=<number[s]>       Max time between two inverse SRDOs\n"
            "  -validation-time=<number[s]>  Max time between an SRDO and its corresponding inverse SRDO\n"
            "  -duration=<number[s]>         Automatically stop after n seconds\n"
            "\n"
        ) << toolName;

        return 1;
    };

    return 0;
}

void safetyListen(String interface, int canId, int invCanId, double cycleTime, double validationTime, double duration)
{
    auto can = CanSocket::open(interface);

    typedef Map<int, double> LastTimes;
    auto lastTimes = LastTimes::create();

    double t0 = System::now();

    uint64_t frameCount = 0;
    uint64_t safetyFrameCount = 0;
    uint64_t inverseSafetyFrameCount = 0;
    uint64_t errorCount = 0;
    double dtMin = std::numeric_limits<double>::max();
    double dtMax = std::numeric_limits<double>::min();
    double dtvMin = std::numeric_limits<double>::max();
    double dtvMax = std::numeric_limits<double>::min();
    double dtAvg = 0;
    double dtvAvg = 0;

    for (auto frame = CanFrame::create(); can->readFrame(frame);)
    {
        double t = System::now();
        double dt = 0, tl = 0;
        if (lastTimes->lookup(frame->canId(), &tl))
            dt = t - tl;

        lastTimes->establish(frame->canId(), t);

        String annotation = "SRDO";

        try {
            if (int(frame->canId()) == invCanId) {
                if (dt > cycleTime) {
                    throw String{Format{"ERROR, SCT EXCEEDED (dt = %%)"} << fixed(dt, 3, 3)};
                }
                else {
                    double ts = 0;
                    double dtv = 0;
                    if (lastTimes->lookup(canId, &ts)) {
                        dtv = t - ts;
                        if (dtv > validationTime) throw String{Format{"ERROR, SRVT EXCEEDED (dtv = %%)"} << dtv};
                        if (dtv < dtvMin) dtvMin = dtv;
                        if (dtv > dtvMax) dtvMax = dtv;
                        dtvAvg += dtv;
                        ++inverseSafetyFrameCount;
                    }
                }
            }
            else if (int(frame->canId()) == canId) {
                if (dt > 0) {
                    if (dt < dtMin) dtMin = dt;
                    if (dt > dtMax) dtMax = dt;
                    dtAvg += dt;
                    ++safetyFrameCount;
                }
            }
        }
        catch (String &error) {
            annotation = error;
            ++errorCount;
        }

        double te = t - t0;
        fout() << fixed(te, 3, 3) << " " << fixed(dt, 3, 3) << ": " << frame << " // " << annotation << nl;

        ++frameCount;

        if (duration > 0 && te >= duration) break;
    }

    dtAvg /= safetyFrameCount;
    dtvAvg /= inverseSafetyFrameCount;

    fout() << nl;
    fout() << "Number of SCT/SRVT errors   : " << errorCount << nl;
    fout() << "Number of frames transferred: " << frameCount << nl;
    fout() << "Number of SRDOs transferred : " << safetyFrameCount << nl;
    fout() << "Achieved cycle times        : " << fixed(dtMin, 3, 3) << " <= dt <= " << fixed(dtMax, 3, 3) << nl;
    fout() << "Achieved validation times   : " << fixed(dtvMin, 3, 3) << " <= dtv <= " << fixed(dtvMax, 3, 3) << nl;
    fout() << "Average cycle time          : " << fixed(dtAvg, 3, 3) << nl;
    fout() << "Average validation time     : " << fixed(dtvAvg, 3, 3) << nl;
}
