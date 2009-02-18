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

#ifndef PONA_INETADDRESS_HPP
#define PONA_INETADDRESS_HPP

#ifndef _MSC_VER
#include <arpa/inet.h> // sockaddr, AF_*
#include <netdb.h> // addrinfo
#include <sys/socket.h> // connect
#else
#include <sys/types.h>
#include <Winsock2.h>
#include <Ws2tcpip.h>
#endif

#include "Atoms.hpp"
#include "String.hpp"
#include "List.hpp"

namespace pona
{

PONA_EXCEPTION(NetworkingException, Exception);

class InetAddress;

typedef List< Ref<InetAddress, Owner> > InetAddressList;

/** \brief Generic internet address (IPv4 or IPv6)
  *
  * The InetAddress class stores the complete connection information of an internet
  * host. In addition the static function InetAddress::query() allows name to
  * address translation and service discovery.
  *   From the design perspective the InetAddress class is a wrapper class around
  * the two low level structures socketaddr and addrinfo.
  *   Internally three different types of address translations are performed:
  * inet_pton(3), inet_ntop(3) and getaddrinfo(3).
  */
class InetAddress: public Instance
{
public:
	enum Family { unspec = AF_UNSPEC, inet4 = AF_INET, inet6 = AF_INET6 };
	enum SocketType { dgram = SOCK_DGRAM, stream = SOCK_STREAM };
	
	/** Initialize object with protocol family and numerical address.
	  * Providing the wildcard address "*" allows to specify an address for
	  * a TCP server, which will listen on all interfaces of the serving host.
	  *
	  * \param family protocol family (inet4 or inet6)
	  * \param address numerical host address or wildcard ("*")
	  * \param port service port
	  */
	InetAddress(int family, String address = String(), int port = 0);
	
	sockaddr* socketAddress() { return &socketAddress_; }
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
	
	/** Return the fully qualified domain name if this object was returned by
	  * InetAddress::query(), else a null string will be returned.
	  */
	String canonicalName() const { return canonicalName_; }
	
	/** Query the complete connection information for given hostname, service and
	  * protocol family. The returned objects will carry fully qualified domain
	  * names (see canonicalName()). The call may block several seconds.
	  *   Depending on supported protocol stacks and service availability in
	  * different protocols (UDP/TCP) and number of network addresses of the
	  * queried host multiple InetAddress objects will be returned.
	  *   The query returns an empty list, if the hostname is unknown, service is
	  * not available or protocol family is not supported by the host.
	  */
	static Ref<InetAddressList, Owner> query( String hostname,
	                                          String service = String(),
	                                          int family = unspec,
	                                          int socketType = 0 );
	
	/** Return the name of this host.
	  */
	static String hostName();
	
private:
	InetAddress(addrinfo* info);
	
	union {
		sockaddr socketAddress_;
		sockaddr_in inet4Address_;
		sockaddr_in6 inet6Address_;
	};
	
	String canonicalName_;
	int family_;
	int socketType_;
	int protocol_;
};

} // namespace pona

#endif // PONA_INETADDRESS_HPP
