/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h> // IFF_UP, etc.
#include <cc/net/SocketAddress>
#include <cc/net/SocketAddressEntry>

namespace cc {
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
    const SocketAddressList *addressList() const { return addressList_; }

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

}} // namespace cc::net
