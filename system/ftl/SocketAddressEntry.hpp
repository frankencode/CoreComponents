/*
 * SocketAddressEntry.hpp -- socket address entry
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
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
	Ref<SocketAddress> address() const;
	Ref<SocketAddress> localAddress() const;
	Ref<SocketAddress> broadcastAddress() const;
	Ref<SocketAddress> destinationAddress() const;
	Ref<SocketAddress> anycastAddress() const;

private:
	friend class NetworkInterface;

	inline static Ref<SocketAddressEntry, Owner> newInstance() {
		return new SocketAddressEntry;
	}
	inline static Ref<SocketAddressEntry, Owner> newInstance(struct sockaddr_in *addr) {
		return new SocketAddressEntry(addr);
	}
	inline static Ref<SocketAddressEntry, Owner> newInstance(struct sockaddr_in6 *addr) {
		return new SocketAddressEntry(addr);
	}

	SocketAddressEntry();
	SocketAddressEntry(struct sockaddr_in *addr);
	SocketAddressEntry(struct sockaddr_in6 *addr);

	Ref<SocketAddress, Owner> localAddress_;
	Ref<SocketAddress, Owner> broadcastAddress_;
	Ref<SocketAddress, Owner> anycastAddress_;
};

} // namespace ftl

#endif // FTL_SOCKETADDRESSENTRY_HPP
