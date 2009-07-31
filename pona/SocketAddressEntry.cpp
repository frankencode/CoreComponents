#include "SocketAddressEntry.hpp"

namespace pona
{

SocketAddressEntry::SocketAddressEntry(struct sockaddr_in* addr): SocketAddress(addr) {}
SocketAddressEntry::SocketAddressEntry(struct sockaddr_in6* addr): SocketAddress(addr) {}
Ref<SocketAddress> SocketAddressEntry::address() const { return const_cast<SocketAddressEntry*>(this); }
Ref<SocketAddress> SocketAddressEntry::localAddress() const { return localAddress_; }
Ref<SocketAddress> SocketAddressEntry::broadcastAddress() const { return broadcastAddress_; }
Ref<SocketAddress> SocketAddressEntry::destinationAddress() const { return broadcastAddress_; }
Ref<SocketAddress> SocketAddressEntry::anycastAddress() const { return anycastAddress_; }

} // namespace pona
