/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXNET_SOCKETADDRESSENTRY_H
#define FLUXNET_SOCKETADDRESSENTRY_H

#include "SocketAddress.h"

namespace flux {
namespace net {

class NetworkInterface;

class SocketAddressEntry: public SocketAddress
{
public:
	SocketAddress *address() const { return const_cast<SocketAddressEntry *>(this); }
	SocketAddress *localAddress() const { return localAddress_; }
	SocketAddress *broadcastAddress() const { return broadcastAddress_; }
	SocketAddress *destinationAddress() const { return broadcastAddress_; }
	SocketAddress *anycastAddress() const { return anycastAddress_; }

private:
	friend class NetworkInterface;

	inline static Ref<SocketAddressEntry> create() {
		return new SocketAddressEntry;
	}
	inline static Ref<SocketAddressEntry> create(struct sockaddr_in *addr) {
		return new SocketAddressEntry(addr);
	}
	inline static Ref<SocketAddressEntry> create(struct sockaddr_in6 *addr) {
		return new SocketAddressEntry(addr);
	}

	SocketAddressEntry();
	SocketAddressEntry(struct sockaddr_in *addr): SocketAddress(addr) {}
	SocketAddressEntry(struct sockaddr_in6 *addr): SocketAddress(addr) {}

	Ref<SocketAddress> localAddress_;
	Ref<SocketAddress> broadcastAddress_;
	Ref<SocketAddress> anycastAddress_;
};

}} // namespace flux::net

#endif // FLUXNET_SOCKETADDRESSENTRY_H
