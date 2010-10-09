#include <ftl/streams>
#include <ftl/sockets>

namespace ftl
{

int main()
{
	Ref<NetworkInterfaceList, Owner> interfaces = NetworkInterface::queryAll(AF_UNSPEC);
	for (NetworkInterfaceList::Index i = interfaces->first(); interfaces->def(i); ++i) {
		Ref<NetworkInterface> interface = interfaces->get(i);
		if (i != interfaces->first()) print("\n");
		print("%%:\n", interface->name());
		print("  Flags: ");
		int flags = interface->flags();
		if (flags & NetworkInterface::Up) print(" up");
		if (flags & NetworkInterface::Running) print(" running");
		if (flags & NetworkInterface::LowerUp) print(" lower_up");
		if (flags & NetworkInterface::Dormant) print(" dormant");
		if (flags & NetworkInterface::Broadcast) print(" bcast");
		if (flags & NetworkInterface::Multicast) print(" mcast");
		if (flags & NetworkInterface::Loopback) print(" loop");
		if (flags & NetworkInterface::PointToPoint) print(" p2p");
		print("\n");
		print("  HwAddr: %hex:%\n", interface->hardwareAddress());
		print("  MTU:    %%\n", interface->mtu());
		Ref<SocketAddressList> addressList = interface->addressList();
		if (addressList) {
			for (SocketAddressList::Index k = addressList->first(); addressList->def(k); ++k) {
				Ref<SocketAddress> address = addressList->at(k);
				print("  Addr:   %%", address->toString());
				Ref<SocketAddressEntry> addressEntry = address;
				if (addressEntry) {
					bool comma = false;
					bool delim = true;
					if (addressEntry->localAddress()) {
						if (delim) { print(" --"); delim = false; }
						print(" Local: %%", addressEntry->localAddress()->toString());
						comma = true;
					}
					if (addressEntry->broadcastAddress()) {
						if (delim) { print(" --"); delim = false; }
						if (comma) print(",");
						print(" Bcast: %%", addressEntry->broadcastAddress()->toString());
						comma = true;
					}
					if (addressEntry->anycastAddress()) {
						if (delim) { print(" --"); delim = false; }
						if (comma) print(",");
						print(" Acast: %%", addressEntry->anycastAddress()->toString());
					}
				}
				print("\n");
			}
		}
	}
	return 0;
}

} // namespace ftl

int main()
{
	return ftl::main();
}
