/****************************************************************************
**
** This file is part of libPONA - The Portable Network Abstraction Library.
**
** Copyright (C) 2007-2009  Frank Mertens
**
** This file is part of a free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License as published
** by the Free Software Foundation, either version 3 of the License,
** or (at your option) any later version.
**
** The library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this libary.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#ifndef PONA_SOCKETADDRESS_HPP
#define PONA_SOCKETADDRESS_HPP

#ifndef _MSC_VER
#include <arpa/inet.h> // sockaddr, AF_*
#include <netdb.h> // addrinfo
#include <sys/socket.h> // connect
#else
#include <sys/types.h>
#include <Winsock2.h>
#include <Ws2tcpip.h>
#endif

#include "atoms"
#include "String.hpp"
#include "List.hpp"

namespace pona
{

PONA_EXCEPTION(NetworkingException, Exception);

class SocketAddress;

typedef List< Ref<SocketAddress, Owner> > InetAddressList;

class SocketAddress: public Instance
{
public:
	/** Initialize object with protocol family and numerical address.
	  * Providing the wildcard address "*" allows to specify an address for
	  * a TCP server, which will listen on all interfaces of the serving host.
	  *
	  * \param family protocol family (AF_UNSPEC, AF_INET or AF_INET6)
	  * \param address numerical host address or wildcard ("*")
	  * \param port service port
	  */
	SocketAddress(int family, String address = String(), int port = 0);
	
	sockaddr* socketAddress() { return &socketAddress_; }
	const sockaddr* socketAddress() const { return &socketAddress_; }
	int socketAddressLength() const;
	
	int family() const { return family_; }
	int socketType() const { return socketType_; }
	int protocol() const { return protocol_; }
	String familyString() const;
	String socketTypeString() const;
	String protocolString() const;
	String addressString() const;
	int port() const;
	void setPort(int port);
	
	/** Query the complete connection information for given host name, service name and
	  * protocol family. The call blocks until the local resolver has resolved the
	  * host name. This may take several seconds.
	  *   Depending on supported protocol stacks and service availability in
	  * different protocols (UDP/TCP) and number of network addresses of the
	  * queried host multiple SocketAddress objects will be returned.
	  *   The query returns an empty list, if the host name is unknown, service is
	  * not available or protocol family is not supported by the host.
	  *   The host name can be a short name relative to the local domain.
	  * The fully qualified domain name (aka canonical name) can be optionally retrieved.
	  */
	static Ref<InetAddressList, Owner> query( String hostName,
	                                          String serviceName = String(),
	                                          int family = AF_UNSPEC,
	                                          int socketType = 0,
	                                          String* canonicalName = 0 );
	
	/** Lookup the host name of given address. Usually a reverse DNS
	  * lookup will be issued, which may take several seconds.
	  */
	String lookupHostName(bool* failed = 0) const;
	
	/** Lookup the service name. In most setups the service name will be looked up
	  * in a local file (/etc/services) and therefore the call returns immediately.
	  */
	String lookupServiceName() const;
	
	/** Returns the name of this host.
	  *   On a properly configured server the host name returned should be a fully
	  * qualified domain name.
	  */
	static String hostName();
	
private:
	SocketAddress(addrinfo* info);
	
	union {
		sockaddr socketAddress_;
		sockaddr_in inet4Address_;
		sockaddr_in6 inet6Address_;
	};
	
	int family_;
	int socketType_;
	int protocol_;
};

} // namespace pona

#endif // PONA_SOCKETADDRESS_HPP
