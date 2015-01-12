/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXNET_SOCKETADDRESSENTRY_H
#define FLUXNET_SOCKETADDRESSENTRY_H

#include <flux/net/SocketAddress>

namespace flux {
namespace net {

class NetworkInterface;

class SocketAddressEntry: public SocketAddress
{
public:
    SocketAddress *address() const { return const_cast<SocketAddressEntry *>(this); }

    int networkMask() const { return networkMask_; }
    SocketAddress *localAddress() const { return localAddress_; }
    SocketAddress *broadcastAddress() const { return broadcastAddress_; }
    SocketAddress *destinationAddress() const { return broadcastAddress_; }
    SocketAddress *anycastAddress() const { return anycastAddress_; }

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
    Ref<SocketAddress> localAddress_;
    Ref<SocketAddress> broadcastAddress_;
    Ref<SocketAddress> anycastAddress_;
};

}} // namespace flux::net

#endif // FLUXNET_SOCKETADDRESSENTRY_H
