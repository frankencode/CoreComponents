/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_SOCKETADDRESSENTRY_H
#define FLUX_SOCKETADDRESSENTRY_H

#include "SocketAddress.h"

namespace flux
{

class NetworkInterface;

class SocketAddressEntry: public SocketAddress
{
public:
	SocketAddress *address() const;
	SocketAddress *localAddress() const;
	SocketAddress *broadcastAddress() const;
	SocketAddress *destinationAddress() const;
	SocketAddress *anycastAddress() const;

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
	SocketAddressEntry(struct sockaddr_in *addr);
	SocketAddressEntry(struct sockaddr_in6 *addr);

	Ref<SocketAddress> localAddress_;
	Ref<SocketAddress> broadcastAddress_;
	Ref<SocketAddress> anycastAddress_;
};

} // namespace flux

#endif // FLUX_SOCKETADDRESSENTRY_H
