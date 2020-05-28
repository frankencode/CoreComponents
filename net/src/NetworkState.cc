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

Ref<NetworkState> NetworkState::query(const String &interfaceName)
{
    return new NetworkState{interfaceName};
}

NetworkState::NetworkState(const String &interfaceName):
    interfaceName_{interfaceName},
    networkMask_{0}
{
    SocketAddress candidate;

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
        for (const SocketAddress &address: interface_->addressList()) {
            if (address->family() != ProtocolFamily::Internet6) continue;
            candidate = address;
            break;
        }
    }
    if (!candidate) {
        for (const SocketAddress &address: interface_->addressList()) {
            if (gateway_ && address->family() != gateway_->family()) continue;
            candidate = address;
            break;
        }
    }

    if (candidate->details()) networkMask_ = candidate->details()->networkMask();
    address_ = candidate;
}

}} // namespace cc::net
