/*
 * Copyright (C) 2007-2016 Frank Mertens.
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
    for (int i = 0; i < routingTable->count(); ++i) {
        RouteInfo *entry = routingTable->at(i);
        if (entry->source() || entry->destination()) continue;
        if (interfaceName_ != "" && entry->outputInterface() != interfaceName_) continue;
        interfaceName_ = entry->outputInterface();
        gateway_ = entry->gateway();
        break;
    }

    Ref<NetworkInterfaceList> interfaceList = NetworkInterface::queryAll(AF_UNSPEC);
    for (int i = 0; i < interfaceList->count(); ++i) {
        NetworkInterface *candidate = interfaceList->at(i);
        if (candidate->name() == interfaceName_) {
            interface_ = candidate;
            const SocketAddressList *addressList = interface_->addressList();
            if (!gateway_) {
                for (int j = 0; j < addressList->count(); ++j) {
                    const SocketAddress *address = addressList->at(j);
                    if (address->family() != AF_INET) continue;
                    candidateAddress = address;
                    break;
                }
            }
            if (!candidateAddress) {
                for (int j = 0; j < addressList->count(); ++j) {
                    const SocketAddress *address = addressList->at(j);
                    if (gateway_ && address->family() != gateway_->family()) continue;
                    candidateAddress = address;
                    break;
                }
            }
        }
    }

    const SocketAddressEntry *entry = cast<const SocketAddressEntry>(candidateAddress);
    if (entry) networkMask_ = entry->networkMask();
    address_ = candidateAddress;
}

}} // namespace cc::net
