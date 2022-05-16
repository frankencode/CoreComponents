/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/SocketAddress>
#include <cc/System>
#include <cc/testing>

int main(int argc, char *argv[])
{
    using namespace cc;

    TestCase {
        "ResolveHostName",
        []{
            String hostName = System::hostName();

            fout("hostName = \"%%\"\n") << hostName;

            String canonicalName;
            List<SocketAddress> list = SocketAddress::queryConnectionInfo(hostName, "http", ProtocolFamily::Unspec, SocketType::Stream, &canonicalName);

            fout("canonicalName = \"%%\"\n") << canonicalName;

            for (const SocketAddress &address: list)
            {
                fout("%% : %% : %% : %% : %% : %% : %%\n")
                    << address.family()
                    << address.toString()
                    << address.port()
                    << address.socketType()
                    << address.protocol()
                    << address.lookupHostName()
                    << address.lookupServiceName();
            }
        }
    };

    return TestSuite{argc, argv}.run();
}
