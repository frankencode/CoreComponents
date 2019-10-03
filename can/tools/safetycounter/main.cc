/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/can/CanSocket>
#include <cc/Arguments>
#include <cc/Thread>
#include <cc/stdio>
#include <cc/exceptions>
#include <cc/debug>

using namespace cc;
using namespace cc::can;

void safetyCounter(String interface, int canId, int invCanId, double refreshTime);

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
            options->insert("refresh-time", 0.5);

            arguments->validate(options);
            arguments->override(options);
        }

        if (arguments->items()->count() > 0) throw HelpRequest{};

        String interface = options->value("interface");
        int canId = options->value("can-id");
        int invCanId = options->value("inv-can-id");
        double refreshTime = options->value("refresh-time");

        if (canId <= 0) {
            ferr() << "Please provide a CAN ID ('-can-id=<number>')" << nl;
            throw HelpRequest{};
        }

        if (refreshTime <= 0) {
            ferr() << "Please provide a refresh time ('-refresh-time')" << nl;
            throw HelpRequest{};
        }

        if (invCanId <= 0)
            invCanId = canId + 1;

        CC_INSPECT(interface);
        CC_INSPECT(hex(canId));
        CC_INSPECT(hex(invCanId));
        CC_INSPECT(fixed(refreshTime, 3, 3));

        safetyCounter(interface, canId, invCanId, refreshTime);
    }
    catch (HelpRequest &) {
        fout(
            "Usage: %% [OPTION]... [DIR]...\n"
            "Emit a counter value using SRDOs.\n"
            "\n"
            "Options:\n"
            "  -interface=<string>        CAN interface to listen on (\"can0\" by default)\n"
            "  -can-id=<number>           CAN ID of the SRDO\n"
            "  -inv-can-id=<number>       CAN ID of the inverse SRDO (defaults to CAN ID + 1)\n"
            "  -refresh-time=<number[s]>  refresh time\n"
            "\n"
        ) << toolName;

        return 1;
    };

    return 0;
}

void safetyCounter(String interface, int canId, int invCanId, double refreshTime)
{
    auto can = CanSocket::open(interface);

    auto frame = CanFrame::create();
    auto invFrame = CanFrame::create();

    frame->setCanId(canId);
    invFrame->setCanId(invCanId);

    uint32_t counter = 0;
    while (true) {
        frame->setValueUInt32(counter);
        frame->updateInverse(invFrame);
        can->writeFrame(frame);
        can->writeFrame(invFrame);
        Thread::sleep(refreshTime);
        ++counter;
    }
}
