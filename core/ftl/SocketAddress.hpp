 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */
#ifndef FTL_SOCKETADDRESS_HPP
#define FTL_SOCKETADDRESS_HPP

#include <arpa/inet.h>
#include <netinet/in.h> // sockaddr_in, etc.
#include <netdb.h> // addrinfo
#include <sys/socket.h> // connect
#include <sys/un.h> // sockaddr_un

#include "string.hpp"
#include "List.hpp"

namespace ftl
{

FTL_EXCEPTION(NetworkingException, Exception);

class SocketAddress;

typedef List< Ref<SocketAddress> > SocketAddressList;

class SocketAddress: public Instance
{
public:
	inline static Ref<SocketAddress> create() {
		return new SocketAddress;
	}

	/** Initialize object with protocol family and numerical address.
	  * Providing the wildcard address "*" allows to specify an address for
	  * a TCP server, which will listen on all interfaces of the serving host.
	  *
	  * \param family protocol family (AF_UNSPEC, AF_INET, AF_INET6 or AF_LOCAL)
	  * \param address numerical host address, wildcard ("*") or file path
	  * \param port service port
	  */
	inline static Ref<SocketAddress> create(int family, string address = string(), int port = 0) {
		return new SocketAddress(family, address, port);
	}

	inline static Ref<SocketAddress> create(struct sockaddr_in *addr) {
		return new SocketAddress(addr);
	}
	inline static Ref<SocketAddress> create(struct sockaddr_in6 *addr) {
		return new SocketAddress(addr);
	}
	inline static Ref<SocketAddress> create(addrinfo *info) {
		return new SocketAddress(info);
	}

	int family() const;
	int socketType() const;
	int protocol() const;
	int port() const;
	void setPort(int port);

	string addressString() const;
	string toString() const;

	int scope() const;
	void setScope(int scope);

	/** Query the complete connection information for given host name, service name and
	  * protocol family. The call blocks until the local resolver has resolved the
	  * host name. This may take several seconds.
	  *   Depending on supported protocol stacks and service availability in
	  * different protocols (UDP/TCP) and number of network addresses of the
	  * queried host multiple SocketAddress objects will be returned.
	  *   An empty list is returned, if the host name is unknown, service is
	  * not available or protocol family is not supported by the host.
	  *   The host name can be a short name relative to the local domain.
	  * The fully qualified domain name (aka canonical name) can be optionally retrieved.
	  */
	static Ref<SocketAddressList> resolve(string hostName, string serviceName = string(), int family = AF_UNSPEC, int socketType = 0, string *canonicalName = 0);

	/** Lookup the host name of given address. Usually a reverse DNS
	  * lookup will be issued, which may take several seconds.
	  */
	string lookupHostName(bool *failed = 0) const;

	/** Lookup the service name. In most setups the service name will be looked up
	  * in a local file (/etc/services) and therefore the call returns immediately.
	  */
	string lookupServiceName() const;

	/** Returns the name of this host.
	  *   On a properly configured server the host name returned should be a fully
	  * qualified domain name.
	  */
	static string hostName();

	struct sockaddr *addr();
	const struct sockaddr *addr() const;
	int addrLen() const;

protected:
	SocketAddress();
	SocketAddress(int family, string address, int port);
	SocketAddress(struct sockaddr_in *addr);
	SocketAddress(struct sockaddr_in6 *addr);
	SocketAddress(addrinfo *info);

	union {
		struct sockaddr addr_;
		struct sockaddr_in inet4Address_;
		struct sockaddr_in6 inet6Address_;
		struct sockaddr_un localAddress_;
	};

	int socketType_;
	int protocol_;
};

} // namespace ftl

#endif // FTL_SOCKETADDRESS_HPP
