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
            options->insert("timeout", -1);
            options->insert("debug", false);
            arguments->validate(options);
            arguments->override(options);
        }

        String interface = options->value("interface");
        int clientId     = options->value("my-id");
        int serverId     = options->value("peer-id");
        int index        = options->value("index");
        int subIndex     = options->value("sub-index");
        int timeout      = options->value("timeout");
        bool debug       = options->value("debug");

        Selector selector{
            static_cast<Selector::Index>(index),
            static_cast<Selector::SubIndex>(subIndex)
        };

        if (debug) {
            CC_INSPECT(interface);
            CC_INSPECT(clientId);
            CC_INSPECT(serverId);
            CC_INSPECT(selector);
            CC_INSPECT(timeout);
        }

        auto media = CanSocket::open(interface);
        auto client = CanClient::create(media, clientId, timeout);
        String data = client->read(serverId, selector);
        fout() << data->hexDump() << nl;
    }
    catch (HelpRequest &) {
        fout(
            "Usage: %% [OPTION]...\n"
            "Read an entry from a CANopen node's dictionary\n"
            "\n"
            "Options:\n"
            "  -interface=<string>  CAN interface to use (\"can0\" by default)\n"
            "  -my-id=<number>      node ID of the client\n"
            "  -peer-id=<number>    node ID of the server\n"
            "  -index=<number>      object dictionary index (0x1000 by default)\n"
            "  -sub-index=<number>  object dictionary sub-index (0x00 by default)\n"
            "  -timeout=<number>    timeout in milliseconds (indefinite by default)\n"
            "  -debug               debug output\n"
        ) << toolName;
    }

    return 0;
}
