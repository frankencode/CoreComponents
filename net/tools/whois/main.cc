/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/net/StreamSocket>
#include <cc/stdio>
#include <cc/LineSource>
#include <cc/exceptions>
#include <cc/Arguments>

using namespace cc;
using namespace cc::net;

String whoisQuery(const SocketAddress *serverAddress, const String &clientRequest, const String &queryKey = "")
{
    auto server = StreamSocket::connect(serverAddress);
    server->write(clientRequest + "\r\n");

    String result;

    if (queryKey != "") {
        for (const String &line: LineSource::open(server)) {
            if (line->count() > 0 && !line->startsWith('%')) {
                auto parts = line->split(":");
                if (parts->count() == 2) {
                    auto key = parts->at(0);
                    auto value = parts->at(1)->trim();
                    if (key == queryKey) {
                        result = value;
                        break;
                    }
                }
            }
        }
    }
    else {
        server->transferTo(stdOut());
    }

    return result;
}

int main(int argc, char **argv)
{
    String toolName = String{argv[0]}->fileName();

    int exitStatus = 0;

    try {
        auto arguments = Arguments::parse(argc, argv, VariantMap::create());

        auto ianaAddress = SocketAddress::resolveHost("whois.iana.org", 43);

        for (const String &item: arguments->items())
        {
            bool addressQuery = false;
            try {
                SocketAddress::parse(item);
                addressQuery = true;
            }
            catch (...) {}

            if (addressQuery) {
                String registry = whoisQuery(ianaAddress, item, "whois");
                if (registry == "") throw Error{2};
                auto registryAddress = SocketAddress::resolveHost(registry, 43);
                whoisQuery(registryAddress, item);
            }
            else {
                auto parts = item->split('.');
                String topLevelDomain = parts->at(parts->count() - 1);
                String domain = parts->at(parts->count() - 2) + "." + topLevelDomain;
                String registry = whoisQuery(ianaAddress, topLevelDomain, "whois");
                if (registry == "") throw Error{3};
                auto registryAddress = SocketAddress::resolveHost(registry, 43);
                whoisQuery(registryAddress, domain);
            }
        }
    }
    catch (Error &error)
    {
        exitStatus = error->exitCode();
    }
    catch (HelpRequest &)
    {
        fout(
            "Usage: %% [OPTION]... [ADDRESS|DOMAIN]...\n"
            "Query WHOIS information for internet addresses and domain names\n"
            "\n"
        ) << toolName;

        exitStatus = 1;
    }
    catch (Exception &ex)
    {
        ferr() << ex << nl;
        exitStatus = 4;
    }

    return exitStatus;
}
