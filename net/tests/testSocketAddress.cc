/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/testing/TestSuite>
#include <cc/stdio>
#include <cc/System>
#include <cc/net/Uri>
#include <cc/net/SocketAddress>

using namespace cc;
using namespace cc::testing;
using namespace cc::net;

String familyToString(int family)
{
    String s("UNKNOWN");
    if (family == AF_INET) s = "INET";
    else if (family == AF_INET6) s = "INET6";
    else if (family == AF_UNSPEC) s = "UNSPEC";
    return s;
}

String socketTypeToString(int type)
{
    String s("UNKNOWN");
    if (type == SOCK_DGRAM) s = "DGRAM";
    else if (type == SOCK_STREAM) s = "STREAM";
    return s;
}

String protocolToString(int protocol)
{
    String s("UNKNOWN");
    if (protocol == IPPROTO_TCP) s = "TCP";
    else if (protocol == IPPROTO_UDP) s = "UDP";
    else s = Format("<%%>") << protocol;
    return s;
}

class ResolveHostName: public TestCase
{
    void run()
    {
        String hostName = System::hostName();

        fout("hostName = \"%%\"\n") << hostName;

        String canonicalName;
        Ref<SocketAddressList> list = SocketAddress::resolve(hostName, "http", AF_UNSPEC, SOCK_STREAM, &canonicalName);

        fout("canonicalName = \"%%\"\n") << canonicalName;

        for (int i = 0; i < list->count(); ++i)
        {
            SocketAddress *address = list->at(i);
            bool failed;

            fout("%% : %% : %% : %% : %% : %% : %%\n")
                << familyToString(address->family())
                << address->toString()
                << address->port()
                << socketTypeToString(address->socketType())
                << protocolToString(address->protocol())
                << address->lookupHostName(&failed)
                << address->lookupServiceName();
        }
    }
};

class ReadLocation: public TestCase
{
    void run()
    {
        auto samples = StringList::create()
            << "192.168.0.1"
            << "128.0.0.1:8080"
            << "::"
            << "[::]:8080";
        for (String s: samples) {
            auto address = SocketAddress::resolve(Uri::parse(s));
            fout("\"%%\" == \"%%\"") << s << address << nl;
            CC_ASSERT(s == address->toString());
        }
    }
};

int main(int argc, char** argv)
{
    CC_TESTSUITE_ADD(ResolveHostName);
    CC_TESTSUITE_ADD(ReadLocation);

    return TestSuite::instance()->run(argc, argv);
}
