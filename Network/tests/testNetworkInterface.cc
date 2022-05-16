/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/NetworkInterface>
#include <cc/testing>
#include <cc/stdio>

int main(int argc, char *argv[])
{
    using namespace cc;

    TestCase {
        "QueryNetworkInterfaces",
        []{
            List<NetworkInterface> interfaceList = NetworkInterface::queryAll();
            for (const NetworkInterface &interface: interfaceList) {
                if (!interfaceList.first(interface)) fout() << nl;
                fout() << interface.name() << ":" << nl;
                fout() << "  Flags: ";
                NetworkInterfaceFlags flags = interface.flags();
                if (flags & NetworkInterfaceFlags::Up) fout() << " up";
                if (flags & NetworkInterfaceFlags::Running) fout() << " running";
                if (flags & NetworkInterfaceFlags::LowerUp) fout() << " lower_up";
                if (flags & NetworkInterfaceFlags::Dormant) fout() << " dormant";
                if (flags & NetworkInterfaceFlags::Broadcast) fout() << " bcast";
                if (flags & NetworkInterfaceFlags::Multicast) fout() << " mcast";
                if (flags & NetworkInterfaceFlags::Loopback) fout() << " loop";
                if (flags & NetworkInterfaceFlags::PointToPoint) fout() << " p2p";
                fout() << nl;
                fout() << "  HwAddr: " << hex(interface.hardwareAddress()).breakUp(2).join('-') << nl;
                fout() << "  MTU:    " << interface.mtu() << nl;

                for (const NetworkLabel &label: interface.labels()) {
                    fout() << "  Addr:   " << label.address();
                    if (label.address().family() == ProtocolFamily::Inet4) {
                        bool comma = false;
                        bool delim = true;
                        if (label.networkMask() > 0)
                            fout() << "/" << label.networkMask();
                        if (label.localAddress()) {
                            if (delim) { fout() << " --"; delim = false; }
                            fout() << " Local: " << label.localAddress();
                            comma = true;
                        }
                        if (label.broadcastAddress()) {
                            if (delim) { fout() << " --"; delim = false; }
                            if (comma) fout() << ",";
                            fout() << " Bcast: " << label.broadcastAddress();
                            comma = true;
                        }
                        if (label.anycastAddress()) {
                            if (delim) { fout() << " --"; delim = false; }
                            if (comma) fout() << ",";
                            fout() << " Acast: " << label.anycastAddress();
                        }
                    }
                    fout() << nl;
                }
            }
        }
    };

    return TestSuite{argc, argv}.run();
}
