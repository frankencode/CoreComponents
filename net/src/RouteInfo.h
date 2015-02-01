/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUX_ROUTEINFO_H
#define FLUX_ROUTEINFO_H

#include <flux/net/SocketAddress>

namespace flux {
namespace net {

class RouteInfo;

typedef List< Ref<RouteInfo> > RouteInfoList;

class RouteInfo: public Object
{
public:
    static Ref<RouteInfoList> queryTable();

    inline int sourceMask() const { return sourceMask_; }
    inline int destinationMask() const { return destinationMask_; }
    inline SocketAddress *source() const { return source_; }
    inline SocketAddress *destination() const { return destination_; }
    inline SocketAddress *gateway() const { return gateway_; }
    inline String inputInterface() const { return inputInterface_; }
    inline String outputInterface() const { return outputInterface_; }
    inline int metric() const { return metric_; }
    inline int priority() const { return priority_; }

    // String toString() const; // TODO
private:
    inline static Ref<RouteInfo> create() { return new RouteInfo; }
    RouteInfo() {}

    int sourceMask_;
    int destinationMask_;
    Ref<SocketAddress> source_;
    Ref<SocketAddress> destination_;
    Ref<SocketAddress> gateway_;
    String inputInterface_;
    String outputInterface_;
    int metric_;
    int priority_;
};

}} // namespace flux::net

#endif // FLUX_ROUTEINFO_H
