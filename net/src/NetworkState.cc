/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/net/RouteInfo>
#include <cc/net/NetworkState>

namespace cc {
namespace net {

Ref<NetworkState> NetworkState::query(String interfaceName)
{
    return new NetworkState(interfaceName);
}

NetworkState::NetworkState(String interfaceName):
    interfaceName_(interfaceName),
    networkMask_(0)
{
    const SocketAddress *candidateAddress = 0;

    Ref<RouteInfoList> routingTable = RouteInfo::queryTable();
    for (const RouteInfo *entry: routingTable) {
        if (entry->source() || entry->destination()) continue;
        if (interfaceName_ != "" && entry->outputInterface() != interfaceName_) continue;
        interfaceName_ = entry->outputInterface();
        gateway_ = entry->gateway();
        break;
    }

    interface_ = NetworkInterface::query(interfaceName_, AF_UNSPEC);
    if (!gateway_) {
        for (const SocketAddress *address: interface_->addressList()) {
            if (address->family() != ProtocolFamily::Internet6) continue;
            candidateAddress = address;
            break;
        }
    }
    if (!candidateAddress) {
        for (const SocketAddress *address: interface_->addressList()) {
            if (gateway_ && address->family() != gateway_->family()) continue;
            candidateAddress = address;
            break;
        }
    }

    const SocketAddressEntry *entry = Object::cast<const SocketAddressEntry *>(candidateAddress);
    if (entry) networkMask_ = entry->networkMask();
    address_ = candidateAddress->copy();
}

}} // namespace cc::net
