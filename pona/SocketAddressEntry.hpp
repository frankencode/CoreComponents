#ifndef PONA_SOCKETADDRESSENTRY_HPP
#define PONA_SOCKETADDRESSENTRY_HPP

#include "atoms"
#include "List.hpp"
#include "SocketAddress.hpp"

namespace pona
{

class NetworkInterface;

class SocketAddressEntry: public SocketAddress
{
public:
	SocketAddressEntry(struct sockaddr_in* addr);
	SocketAddressEntry(struct sockaddr_in6* addr);
	
	Ref<SocketAddress> address() const;
	Ref<SocketAddress> localAddress() const;
	Ref<SocketAddress> broadcastAddress() const;
	Ref<SocketAddress> destinationAddress() const;
	Ref<SocketAddress> anycastAddress() const;
	
private:
	friend class NetworkInterface;
	
	SocketAddressEntry();
	
	Ref<SocketAddress, Owner> localAddress_;
	Ref<SocketAddress, Owner> broadcastAddress_;
	Ref<SocketAddress, Owner> anycastAddress_;
};

} // namespace pona

#endif // PONA_SOCKETADDRESSENTRY_HPP
