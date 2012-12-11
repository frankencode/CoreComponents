/*
 * SocketAddressEntry.cpp -- socket address entry
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */

#include "SocketAddressEntry.hpp"

namespace ftl
{

SocketAddressEntry::SocketAddressEntry(struct sockaddr_in *addr): SocketAddress(addr) {}
SocketAddressEntry::SocketAddressEntry(struct sockaddr_in6 *addr): SocketAddress(addr) {}
Ref<SocketAddress> SocketAddressEntry::address() const { return const_cast<SocketAddressEntry*>(this); }
Ref<SocketAddress> SocketAddressEntry::localAddress() const { return localAddress_; }
Ref<SocketAddress> SocketAddressEntry::broadcastAddress() const { return broadcastAddress_; }
Ref<SocketAddress> SocketAddressEntry::destinationAddress() const { return broadcastAddress_; }
Ref<SocketAddress> SocketAddressEntry::anycastAddress() const { return anycastAddress_; }

} // namespace ftl
