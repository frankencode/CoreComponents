/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/testing/TestSuite>
#include <flux/stdio>
#include <flux/net/NetworkInterface>

using namespace flux;
using namespace flux::testing;
using namespace flux::net;

class QueryNetworkInterfaces: public TestCase
{
    void run()
    {
        Ref<NetworkInterfaceList> interfaces = NetworkInterface::queryAll(AF_UNSPEC);
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
            fout() << "  HwAddr: " << hex(interface->hardwareAddress(), 12)->breakUp(2)->join("-") << nl;
            fout() << "  MTU:    " << interface->mtu() << nl;
            SocketAddressList *addressList = interface->addressList();
            if (addressList) {
                for (int k = 0; k < addressList->count(); ++k) {
                    SocketAddress *address = addressList->at(k);
                    fout() << "  Addr:   " << address;
                    SocketAddressEntry *entry = cast<SocketAddressEntry>(address);
                    if (entry) {
                        bool comma = false;
                        bool delim = true;
                        if (entry->networkMask() > 0)
                            fout() << "/" << entry->networkMask();
                        if (entry->localAddress()) {
                            if (delim) { fout() << " --"; delim = false; }
                            fout() << " Local: " << entry->localAddress();
                            comma = true;
                        }
                        if (entry->broadcastAddress()) {
                            if (delim) { fout() << " --"; delim = false; }
                            if (comma) fout() << ",";
                            fout() << " Bcast: " << entry->broadcastAddress();
                            comma = true;
                        }
                        if (entry->anycastAddress()) {
                            if (delim) { fout() << " --"; delim = false; }
                            if (comma) fout() << ",";
                            fout() << " Acast: " << entry->anycastAddress();
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
    FLUX_TESTSUITE_ADD(QueryNetworkInterfaces);

    return testSuite()->run(argc, argv);
}
