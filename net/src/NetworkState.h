/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXNET_NETWORKSTATE_H
#define FLUXNET_NETWORKSTATE_H

#include <flux/net/NetworkInterface>

namespace flux {
namespace net {

/** \brief convenience class to query the current network configuration state
  */
class NetworkState: public Object
{
public:
    static Ref<NetworkState> query(String interfaceName = "");

    inline String interfaceName() const { return interfaceName_; }

    inline NetworkInterface *interface() const { return interface_; }

    /** size of the network mask (IPv4 only)
      */
    inline int networkMask() const { return networkMask_; }

    /** configured address
      */
    inline SocketAddress *address() const { return address_; }

    /** configured gateway
      */
    inline SocketAddress *gateway() const { return gateway_; }

private:
    NetworkState(String interface);

    String interfaceName_;
    Ref<NetworkInterface> interface_;
    int networkMask_;
    Ref<SocketAddress> address_;
    Ref<SocketAddress> gateway_;
};

}} // namespace flux::net

#endif // FLUXNET_NETWORKSTATE_H
