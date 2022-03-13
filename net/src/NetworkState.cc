/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/NetworkState>
#include <cc/NetworkRoute>

namespace cc {

struct NetworkState::State: public Object::State
{
    State(const String &interfaceName):
        interfaceName_{interfaceName}
    {
        List<NetworkRoute> routingTable = NetworkRoute::queryAll();
        for (const NetworkRoute &entry: routingTable) {
            if (entry.source() || entry.destination()) continue;
            if (interfaceName_ != "" && entry.outputInterface() != interfaceName_) continue;
            interfaceName_ = entry.outputInterface();
            gateway_ = entry.gateway();
            break;
        }

        interface_ = NetworkInterface::query(interfaceName_, ProtocolFamily::Unspec);
        if (!gateway_) {
            for (const NetworkLabel &label: interface_.labels()) {
                if (label.address().family() != ProtocolFamily::Inet6) continue;
                address_ = label.address();
                networkMask_ = label.networkMask();
                return;
            }
        }

        for (const NetworkLabel &label: interface_.labels()) {
            if (gateway_ && label.address().family() != gateway_.family()) continue;
            address_ = label.address();
            networkMask_ = label.networkMask();
            break;
        }
    }

    String interfaceName_;
    NetworkInterface interface_;
    int networkMask_ { 0 };
    SocketAddress address_;
    SocketAddress gateway_;
};

NetworkState::NetworkState(const String &interfaceName):
    Object{new State{interfaceName}}
{}

String NetworkState::interfaceName() const
{
    return me().interfaceName_;
}

const NetworkInterface &NetworkState::interface() const
{
    return me().interface_;
}

int NetworkState::networkMask() const
{
    return me().networkMask_;
}

SocketAddress NetworkState::address() const
{
    return me().address_;
}

SocketAddress NetworkState::gateway() const
{
    return me().gateway_;
}

const NetworkState::State &NetworkState::me() const
{
    return Object::me.as<State>();
}

} // namespace cc
