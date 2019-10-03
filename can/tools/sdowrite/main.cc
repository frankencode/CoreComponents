/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/can/CanSocket>
#include <cc/can/CanClient>
#include <cc/Arguments>
#include <cc/ByteSink>
#include <cc/stdio>
#include <cc/debug>

using namespace cc;
using namespace cc::can;

int main(int argc, char **argv)
{
    String toolName = String(argv[0])->fileName();

    try {
        Ref<Arguments> arguments = Arguments::parse(argc, argv);

        Ref<VariantMap> options = VariantMap::create();
        {
            options->insert("interface", "can0");
            options->insert("my-id", 0x7F);
            options->insert("peer-id", 1);
            options->insert("index", 0x1000);
            options->insert("sub-index", 0);
            options->insert("hex-value", "");
            options->insert("string-value", "");
            options->insert("int-value", 0);
            options->insert("timeout", -1);
            options->insert("debug", false);
            arguments->validate(options);
            arguments->override(options);
        }

        String interface   = options->value("interface");
        int clientId       = options->value("my-id");
        int serverId       = options->value("peer-id");
        int index          = options->value("index");
        int subIndex       = options->value("sub-index");
        String hexValue    = options->value("hex-value");
        String stringValue = options->value("string-value");
        int intValue       = options->value("int-value");
        int timeout        = options->value("timeout");
        bool debug         = options->value("debug");

        Selector selector{
            static_cast<Selector::Index>(index),
            static_cast<Selector::SubIndex>(subIndex)
        };

        String newData;

        if (hexValue->count() > 0)
        {
            auto deHex = [](uint8_t digit) -> uint8_t {
                if ('0' <= digit && digit <= '9') return digit - '0';
                if ('a' <= digit && digit <= 'f') return 0xA + (digit - 'a');
                if ('A' <= digit && digit <= 'F') return 0xA + (digit - 'A');
                return 0;
            };

            auto hexPairs = hexValue->split(".");
            newData = String::allocate(hexPairs->count());
            if (debug)
                CC_INSPECT(hexPairs->count());
            for (int i = 0; i < hexPairs->count(); ++i) {
                mutate(newData)->byteAt(i) =
                    (deHex(hexPairs->at(i)->byteAt(0)) << 4) |
                    deHex(hexPairs->at(i)->byteAt(1));
            }
        }
        else if (stringValue->count() > 0) {
            newData = stringValue;
        }
        else {
            newData = String::allocate(4);
            ByteSink::open(mutate(newData))->writeInt32(intValue);
        }

        if (debug) {
            CC_INSPECT(interface);
            CC_INSPECT(clientId);
            CC_INSPECT(serverId);
            CC_INSPECT(selector);
            CC_INSPECT(newData->hexDump());
            CC_INSPECT(timeout);
        }

        auto client = CanClient::create(CanSocket::open(interface), clientId, timeout);
        client->write(serverId, selector, newData);
    }
    catch (HelpRequest &) {
        fout(
            "Usage: %% [OPTION]...\n"
            "Write an entry into a node's CANopen dictionary\n"
            "\n"
            "Options:\n"
            "  -interface=<string>     CAN interface to use (\"can0\" by default)\n"
            "  -my-id=<number>         node ID of the client\n"
            "  -peer-id=<number>       node ID of the server\n"
            "  -index=<number>         object dictionary index (0x1000 by default)\n"
            "  -sub-index=<number>     object dictionary sub-index (0x00 by default)\n"
            "  -hex-value=<string>     new value to write (hex coded, e.g. '\"02.20.AB\"')\n"
            "  -string-value=<string>  new value to write (hex coded, e.g. \'save\')\n"
            "  -int-value=<number>     new value to write (hex coded, e.g. -2)\n"
            "  -timeout=<number>       timeout in milliseconds (indefinite by default)\n"
            "  -debug                  debug output\n"
        ) << toolName;
    }

    return 0;
}
