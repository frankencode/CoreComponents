/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/net/NetworkInterface>

namespace cc {
namespace net {

/** \class NetworkState NetworkState.h cc/net/NetworkState
  * \brief convenience class to query the current network configuration state
  */
class NetworkState: public Object
{
public:
    static Ref<NetworkState> query(String interfaceName = "");

    inline String interfaceName() const { return interfaceName_; }

    inline const NetworkInterface *interface() const { return interface_; }

    /** size of the network mask (IPv4 only)
      */
    inline int networkMask() const { return networkMask_; }

    /** configured address
      */
    inline const SocketAddress *address() const { return address_; }

    /** configured gateway
      */
    inline const SocketAddress *gateway() const { return gateway_; }

private:
    NetworkState(String interface);

    String interfaceName_;
    Ref<NetworkInterface> interface_;
    int networkMask_;
    Ref<SocketAddress> address_;
    Ref<SocketAddress> gateway_;
};

}} // namespace cc::net
