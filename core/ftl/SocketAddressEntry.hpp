 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
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

	inline static Ref<SocketAddressEntry, Owner> create() {
		return new SocketAddressEntry;
	}
	inline static Ref<SocketAddressEntry, Owner> create(struct sockaddr_in *addr) {
		return new SocketAddressEntry(addr);
	}
	inline static Ref<SocketAddressEntry, Owner> create(struct sockaddr_in6 *addr) {
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
