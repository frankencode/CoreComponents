/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/net/SocketAddress>

namespace cc {
namespace net {

class NetworkInterface;

/** \class SocketAddressEntry SocketAddressEntry.h cc/net/SocketAddressEntry
  * \brief IPv4 address information associated with a network interface
  */
class SocketAddressEntry: public SocketAddress
{
public:
    /// network address associated with this interface
    const SocketAddress *address() const { return const_cast<SocketAddressEntry *>(this); }

    /// size of the subnet mask
    int networkMask() const { return networkMask_; }

    /// local address
    const SocketAddress *localAddress() const { return localAddress_; }

    /// broadcast address
    const SocketAddress *broadcastAddress() const { return broadcastAddress_; }

    /// destination address in cases this is a point-to-point link
    const SocketAddress *destinationAddress() const { return broadcastAddress_; }

    /// anycast address
    const SocketAddress *anycastAddress() const { return anycastAddress_; }

private:
    friend class NetworkInterface;

    inline static Ref<SocketAddressEntry> create() {
        return new SocketAddressEntry;
    }
    inline static Ref<SocketAddressEntry> create(struct sockaddr_in *addr, int networkMask = 0) {
        return new SocketAddressEntry(addr, networkMask);
    }
    inline static Ref<SocketAddressEntry> create(struct sockaddr_in6 *addr) {
        return new SocketAddressEntry(addr);
    }

    SocketAddressEntry(): networkMask_(0) {}
    SocketAddressEntry(struct sockaddr_in *addr, int networkMask = 0): SocketAddress(addr), networkMask_(networkMask) {}
    SocketAddressEntry(struct sockaddr_in6 *addr): SocketAddress(addr), networkMask_(0) {}

    int networkMask_;
    Ref<const SocketAddress> localAddress_;
    Ref<const SocketAddress> broadcastAddress_;
    Ref<const SocketAddress> anycastAddress_;
};

}} // namespace cc::net
