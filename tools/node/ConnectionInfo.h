/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Object>

namespace cc {
namespace net {
    class SocketAddress;
}}

namespace ccnode {

using namespace cc;
using namespace cc::net;

class ConnectionInfo: public Object
{
public:
    static Ref<ConnectionInfo> create(const SocketAddress *remoteAddress);

    const SocketAddress *remoteAddress() const { return remoteAddress_; }
    int priority() const { return priority_; }
    void setPriority(int newPriority) { priority_ = newPriority; }

    uint64_t originAddress() const { return originAddress_; }
    double arrivalTime() const { return arrivalTime_; }
    double departureTime() const { return departureTime_; }

    void updateDepartureTime();

private:
    ConnectionInfo(const SocketAddress *remoteAddress);

    Ref<const SocketAddress> remoteAddress_;
    int priority_;
    uint64_t originAddress_;
    double arrivalTime_;
    double departureTime_;
};

} // namespace ccnode
