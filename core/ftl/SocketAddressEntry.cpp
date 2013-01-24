 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */

#include "SocketAddressEntry.hpp"

namespace ftl
{

SocketAddressEntry::SocketAddressEntry(struct sockaddr_in *addr): SocketAddress(addr) {}
SocketAddressEntry::SocketAddressEntry(struct sockaddr_in6 *addr): SocketAddress(addr) {}
SocketAddress *SocketAddressEntry::address() const { return const_cast<SocketAddressEntry*>(this); }
SocketAddress *SocketAddressEntry::localAddress() const { return localAddress_; }
SocketAddress *SocketAddressEntry::broadcastAddress() const { return broadcastAddress_; }
SocketAddress *SocketAddressEntry::destinationAddress() const { return broadcastAddress_; }
SocketAddress *SocketAddressEntry::anycastAddress() const { return anycastAddress_; }

} // namespace ftl
