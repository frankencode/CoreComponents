/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXNET_NETWORKINTERFACE_H
#define FLUXNET_NETWORKINTERFACE_H

#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h> // IFF_UP, etc.
#include <flux/net/SocketAddress>
#include <flux/net/SocketAddressEntry>

namespace flux {
namespace net {

class RouteInfo;
class NetworkInterface;

typedef List< Ref<NetworkInterface> > NetworkInterfaceList;

/** \brief Query network interface configuration: addresses, flags, etc.
  */
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

    static Ref<NetworkInterfaceList> queryAll(int family = AF_INET6);

    String name() const { return name_; }
    int index() const { return index_; }
    unsigned type() const { return type_; }
    unsigned flags() const { return flags_; }
    uint64_t hardwareAddress() const { return hardwareAddress_; }
    uint32_t mtu() const { return mtu_; }
    SocketAddressList *addressList() const { return addressList_; }

private:
    inline static Ref<NetworkInterface> create() {
        return new NetworkInterface;
    }

    NetworkInterface();

    #ifdef __linux
    friend class RouteInfo;
    static Ref<NetworkInterface> getLink(NetworkInterfaceList *list = 0, int index = -1);
    inline static Ref<NetworkInterface> getLink(int index) { return getLink(0, index); }
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

}} // namespace flux::net

#endif // FLUXNET_NETWORKINTERFACE_H
