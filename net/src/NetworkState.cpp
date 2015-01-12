/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/net/RouteInfo>
#include <flux/net/NetworkState>

namespace flux {
namespace net {

Ref<NetworkState> NetworkState::query(String interfaceName)
{
    return new NetworkState(interfaceName);
}

NetworkState::NetworkState(String interfaceName):
    interfaceName_(interfaceName),
    networkMask_(0)
{
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
            Ref<SocketAddressList> addressList = interface_->addressList();
            if (!gateway_) {
                for (int j = 0; j < addressList->count(); ++j) {
                    SocketAddress *address = addressList->at(j);
                    if (address->family() != AF_INET) continue;
                    address_ = address;
                    break;
                }
            }
            if (!address_) {
                for (int j = 0; j < addressList->count(); ++j) {
                    SocketAddress *address = addressList->at(j);
                    if (gateway_ && address->family() != gateway_->family()) continue;
                    address_ = address;
                    break;
                }
            }
        }
    }

    Ref<SocketAddressEntry> entry = cast<SocketAddressEntry>(address_);
    if (entry)
        networkMask_ = entry->networkMask();
}

}} // namespace flux::net
