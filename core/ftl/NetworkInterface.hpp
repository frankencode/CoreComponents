 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */
#ifndef FTL_NETWORKINTERFACE_HPP
#define FTL_NETWORKINTERFACE_HPP

#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h> // IFF_UP, etc.
#include "SocketAddress.hpp"
#include "SocketAddressEntry.hpp"

namespace ftl
{

class NetworkInterface;

typedef List< hook<NetworkInterface> > NetworkInterfaceList;

class NetworkInterface: public Instance
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

	inline static hook<NetworkInterface> create() {
		return new NetworkInterface;
	}

	string name() const;
	int index() const;
	unsigned type() const;
	unsigned flags() const;
	uint64_t hardwareAddress() const;
	uint32_t mtu() const;
	SocketAddressList *addressList() const;

	static hook<NetworkInterfaceList> queryAll(int family = AF_INET6);

private:
	NetworkInterface();
	#ifdef __linux
	static bool getLink(NetworkInterfaceList *list, int index = -1);
	static hook<NetworkInterfaceList> queryAllIoctl(int family);
	#endif

	string name_;
	int index_;
	unsigned type_;
	unsigned flags_;
	uint64_t hardwareAddress_;
	uint32_t mtu_;
	hook<SocketAddressList> addressList_;
};

} // namespace ftl

#endif // FTL_NETWORKINTERFACE_HPP
