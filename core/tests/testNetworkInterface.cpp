#include <flux/stdio.h>
#include <flux/NetworkInterface.h>

using namespace flux;

int main()
{
	Ref<NetworkInterfaceList> interfaces = NetworkInterface::queryAll(AF_UNSPEC);
	for (int i = 0; i < interfaces->size(); ++i) {
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
			for (int k = 0; k < addressList->size(); ++k) {
				SocketAddress *address = addressList->at(k);
				fout() << "  Addr:   " << address;
				SocketAddressEntry *addressEntry = cast<SocketAddressEntry>(address);
				if (addressEntry) {
					bool comma = false;
					bool delim = true;
					if (addressEntry->localAddress()) {
						if (delim) { fout() << " --"; delim = false; }
						fout() << " Local: " << addressEntry->localAddress();
						comma = true;
					}
					if (addressEntry->broadcastAddress()) {
						if (delim) { fout() << " --"; delim = false; }
						if (comma) fout() << ",";
						fout() << " Bcast: " << addressEntry->broadcastAddress();
						comma = true;
					}
					if (addressEntry->anycastAddress()) {
						if (delim) { fout() << " --"; delim = false; }
						if (comma) fout() << ",";
						fout() << " Acast: " << addressEntry->anycastAddress();
					}
				}
				fout() << nl;
			}
		}
	}

	return 0;
}
