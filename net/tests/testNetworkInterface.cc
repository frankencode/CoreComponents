/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/testing/TestSuite>
#include <cc/stdio>
#include <cc/net/NetworkInterface>

using namespace cc;
using namespace cc::testing;
using namespace cc::net;

class QueryNetworkInterfaces: public TestCase
{
    void run()
    {
        Ref<NetworkInterfaceList> interfaces = NetworkInterface::queryAll();
        for (int i = 0; i < interfaces->count(); ++i) {
            NetworkInterface *interface = interfaces->at(i);
            if (i != 0) fout() << nl;
            fout() << interface->name() << ":" << nl;
            fout() << "  Flags: ";
            int flags = interface->flags();
            if (flags & NetworkInterface::Up) fout() << " up";
            if (flags & NetworkInterface::Running) fout() << " running";
            if (flags & NetworkInterface::LowerUp) fout() << " lower_up";
            if (flags & NetworkInterface::Dormant) fout() << " dormant";
            if (flags & NetworkInterface::Broadcast) fout() << " bcast";
            if (flags & NetworkInterface::Multicast) fout() << " mcast";
            if (flags & NetworkInterface::Loopback) fout() << " loop";
            if (flags & NetworkInterface::PointToPoint) fout() << " p2p";
            fout() << nl;
            fout() << "  HwAddr: " << interface->hardwareAddress()->toHex()->breakUp(2)->join("-") << nl;
            fout() << "  MTU:    " << interface->mtu() << nl;
            const SocketAddressList *addressList = interface->addressList();
            if (addressList) {
                for (int k = 0; k < addressList->count(); ++k) {
                    const SocketAddress &address = addressList->at(k);
                    fout() << "  Addr:   " << address;
                    const SocketAddressDetails *details = address->details();
                    if (details) {
                        bool comma = false;
                        bool delim = true;
                        if (details->networkMask() > 0)
                            fout() << "/" << details->networkMask();
                        if (details->localAddress()) {
                            if (delim) { fout() << " --"; delim = false; }
                            fout() << " Local: " << details->localAddress();
                            comma = true;
                        }
                        if (details->broadcastAddress()) {
                            if (delim) { fout() << " --"; delim = false; }
                            if (comma) fout() << ",";
                            fout() << " Bcast: " << details->broadcastAddress();
                            comma = true;
                        }
                        if (details->anycastAddress()) {
                            if (delim) { fout() << " --"; delim = false; }
                            if (comma) fout() << ",";
                            fout() << " Acast: " << details->anycastAddress();
                        }
                    }
                    fout() << nl;
                }
            }
        }
    }
};

int main(int argc, char** argv)
{
    CC_TESTSUITE_ADD(QueryNetworkInterfaces);

    return TestSuite::instance()->run(argc, argv);
}
