/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FKIT_NETWORKINTERFACE_H
#define FKIT_NETWORKINTERFACE_H

#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h> // IFF_UP, etc.
#include "SocketAddress.h"
#include "SocketAddressEntry.h"

namespace fkit
{

class NetworkInterface;

typedef List< Ref<NetworkInterface> > NetworkInterfaceList;

class NetworkInterface: public Object
{
public:
	enum Flags {
		Up           = IFF_UP,
		#ifdef IFF_RUNNING
		Running      = IFF_RUNNING,
		#else
		Running      = 0,
		#endif
		#ifdef IFF_LOWER_UP
		LowerUp      = IFF_LOWER_UP,
		#else
		LowerUp      = 0,
		#endif
		#ifdef IFF_DORMANT
		Dormant      = IFF_DORMANT,
		#else
		Dormant      = 0,
		#endif
		Broadcast    = IFF_BROADCAST,
		Multicast    = IFF_MULTICAST,
		Loopback     = IFF_LOOPBACK,
		PointToPoint = IFF_POINTOPOINT
	};

	inline static Ref<NetworkInterface> create() {
		return new NetworkInterface;
	}

	String name() const;
	int index() const;
	unsigned type() const;
	unsigned flags() const;
	uint64_t hardwareAddress() const;
	uint32_t mtu() const;
	SocketAddressList *addressList() const;

	static Ref<NetworkInterfaceList> queryAll(int family = AF_INET6);

private:
	NetworkInterface();
	#ifdef __linux
	static bool getLink(NetworkInterfaceList *list, int index = -1);
	static Ref<NetworkInterfaceList> queryAllIoctl(int family);
	#endif

	String name_;
	int index_;
	unsigned type_;
	unsigned flags_;
	uint64_t hardwareAddress_;
	uint32_t mtu_;
	Ref<SocketAddressList> addressList_;
};

} // namespace fkit

#endif // FKIT_NETWORKINTERFACE_H
