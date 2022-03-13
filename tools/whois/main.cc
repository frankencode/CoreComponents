/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ClientSocket>
#include <cc/LineSource>
#include <cc/Arguments>
#include <cc/exceptions>
#include <cc/stdio>

using namespace cc;

String whoisQuery(const SocketAddress &address, const String &request, const String &key = "")
{
    ClientSocket client{address};

    client.wait(IoEvent::Established);
    client.write(request + "\r\n");

    String result;

    if (key != "") {
        for (const String &line: LineSource{client}) {
            if (line.count() > 0 && !line.startsWith('%')) {
                auto parts = line.split(":");
                if (parts.count() == 2) {
                    auto candidate = parts.at(0);
                    auto value = parts.at(1).trimmed();
                    if (candidate == key) {
                        result = value;
                        break;
                    }
                }
            }
        }
    }
    else {
        client.transferTo(IoStream::output());
    }

    return result;
}

int main(int argc, char *argv[])
{
    String toolName = String{argv[0]}.baseName();

    int exitStatus = 0;

    try {
        List<String> items = Arguments{argc, argv}.read();

        auto ianaAddress = SocketAddress::resolveHostName("whois.iana.org", 43);

        for (const String &item: items)
        {
            bool addressQuery = false;
            try {
                SocketAddress{item};
                addressQuery = true;
            }
            catch (...) {}

            if (addressQuery) {
                String registry = whoisQuery(ianaAddress, item, "whois");
                if (registry == "") throw Error{2};
                auto registryAddress = SocketAddress::resolveHostName(registry, 43);
                whoisQuery(registryAddress, item);
            }
            else {
                auto parts = item.split('.');
                String topLevelDomain = parts.at(parts.count() - 1);
                String domain = parts.at(parts.count() - 2) + "." + topLevelDomain;
                String registry = whoisQuery(ianaAddress, topLevelDomain, "whois");
                if (registry == "") throw Error{3};
                auto registryAddress = SocketAddress::resolveHostName(registry, 43);
                whoisQuery(registryAddress, domain);
            }
        }
    }
    catch (Error &error)
    {
        exitStatus = error.exitCode();
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
