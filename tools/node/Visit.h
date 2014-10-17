/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXNODE_VISIT_H
#define FLUXNODE_VISIT_H

#include <flux/Object>

namespace flux {
namespace net {
    class SocketAddress;
}}

namespace fluxnode {

using namespace flux;
using namespace flux::net;

class ConnectionManager;

class Visit: public Object
{
public:
    static Ref<Visit> create(SocketAddress *remoteAddress);

    inline SocketAddress *remoteAddress() const { return remoteAddress_; }
    inline int priority() const { return priority_; }

    inline uint64_t originAddress() const { return originAddress_; }
    inline double arrivalTime() const { return arrivalTime_; }
    inline double departureTime() const { return departureTime_; }

    void updateDepartureTime();

private:
    friend class ConnectionManager;

    Visit(SocketAddress *remoteAddress);
    inline void setPriority(int newPriority) { priority_ = newPriority; }

    Ref<SocketAddress> remoteAddress_;
    int priority_;
    uint64_t originAddress_;
    double arrivalTime_;
    double departureTime_;
};

} // namespace fluxnode

#endif // FLUXNODE_VISIT_H
