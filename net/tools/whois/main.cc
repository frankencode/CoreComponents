/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/debug> // DEBUG
#include <cc/stdio>
#include <cc/exceptions>
#include <cc/Arguments>
#include <cc/LineSource>
#include <cc/net/StreamSocket>

using namespace cc;
using namespace cc::net;

int main(int argc, char **argv)
{
    String toolName = String(argv[0])->fileName();

    try {
        auto arguments = Arguments::parse(argc, argv, VariantMap::create());

        auto ianaAddress = SocketAddress::resolve("whois.iana.org");
        ianaAddress->setPort(43);
        CC_INSPECT(ianaAddress);
        for (const String &address: arguments->items()) {
            SocketAddress::parse(address);
            fout() << address << nl;
            String registry;
            {
                auto iana = StreamSocket::connect(ianaAddress);
                iana->write(address + "\r\n");
                for (const String &line: LineSource::open(iana)) {
                    ferr() << line << nl;
                    if (line->count() > 0 && !line->startsWith('%')) {
                        auto parts = line->split(":");
                        if (parts->count() == 2) {
                            auto key = parts->at(0);
                            auto value = parts->at(1)->trim();
                            if (key == "whois") registry = value;
                        }
                    }
                }
            }
            CC_INSPECT(registry);

            if (registry == "") {
                throw Error{
                    Format{"Failed to obtain name of regional internet registry for \"%%\""} << address,
                    2
                };
            }

            auto rirAddress = SocketAddress::resolve(registry);
            rirAddress->setPort(43);
            CC_INSPECT(rirAddress);
            {
                auto rir = StreamSocket::connect(rirAddress);
                rir->write(address + "\r\n");
                fout() << rir->readAll();
            }
        }
    }
    catch (Error &error) {
        ferr() << error->message() << nl;
        return error->exitCode();
    }
    catch (HelpRequest &) {
        fout(
            "Usage: %% [OPTION]... [address]...\n"
            "Query information about internet addresses (IPv4 or IPv6)\n"
            "\n"
        ) << toolName;
        return 1;
    }

    return 0;
}
