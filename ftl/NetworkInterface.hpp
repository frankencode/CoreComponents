/*
 * NetworkInterface.hpp -- network interface
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */
#ifndef FTL_NETWORKINTERFACE_HPP
#define FTL_NETWORKINTERFACE_HPP

#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h> // IFF_UP, etc.
#include "atoms"
#include "SocketAddress.hpp"
#include "SocketAddressEntry.hpp"

namespace ftl
{

class NetworkInterface;

typedef List< Ref<NetworkInterface, Owner> > NetworkInterfaceList;

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
	
	String name() const;
	int index() const;
	unsigned type() const;
	unsigned flags() const;
	uint64_t hardwareAddress() const;
	uint32_t mtu() const;
	Ref<SocketAddressList> addressList() const;
	
	static Ref<NetworkInterfaceList, Owner> queryAll(int family = AF_INET6);
	
private:
	NetworkInterface();
	#ifdef __linux
	static Ref<NetworkInterfaceList, Owner> queryAllIoctl(int family);
	#endif
	
	String name_;
	int index_;
	unsigned type_;
	unsigned flags_;
	uint64_t hardwareAddress_;
	uint32_t mtu_;
	Ref<SocketAddressList, Owner> addressList_;
};

} // namespace ftl

#endif // FTL_NETWORKINTERFACE_HPP
