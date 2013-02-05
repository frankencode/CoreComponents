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

#include "SocketAddress.hpp"

namespace ftl
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

	inline static hook<SocketAddressEntry> create() {
		return new SocketAddressEntry;
	}
	inline static hook<SocketAddressEntry> create(struct sockaddr_in *addr) {
		return new SocketAddressEntry(addr);
	}
	inline static hook<SocketAddressEntry> create(struct sockaddr_in6 *addr) {
		return new SocketAddressEntry(addr);
	}

	SocketAddressEntry();
	SocketAddressEntry(struct sockaddr_in *addr);
	SocketAddressEntry(struct sockaddr_in6 *addr);

	hook<SocketAddress> localAddress_;
	hook<SocketAddress> broadcastAddress_;
	hook<SocketAddress> anycastAddress_;
};

} // namespace ftl

#endif // FTL_SOCKETADDRESSENTRY_HPP
