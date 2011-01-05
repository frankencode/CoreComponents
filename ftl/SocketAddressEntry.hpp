/*
 * SocketAddressEntry.hpp -- socket address entry
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */
#ifndef FTL_SOCKETADDRESSENTRY_HPP
#define FTL_SOCKETADDRESSENTRY_HPP

#include "atoms"
#include "SocketAddress.hpp"

namespace ftl
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

} // namespace ftl

#endif // FTL_SOCKETADDRESSENTRY_HPP
