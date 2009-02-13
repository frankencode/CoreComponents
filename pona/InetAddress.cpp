/****************************************************************************
**
** This file is part of libPONA - The Portable Network Abstractions Library.
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

#ifdef _MSC_VER
#include "Windows/WsaAwareness.hpp"
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h> // inet_pton, inet_ntop
#include <netinet/in.h> // htons, ntohs
#include <stdlib.h> // malloc
#include <unistd.h> // gethostname
#include <netdb.h> // getaddrinfo, freeaddrinfo
#include <string.h> // memset, memcpy
#include <errno.h>
#endif

#include "LinePrinter.hpp"
#include "InetAddress.hpp"

#ifdef _MSC_VER
#include "Windows/inet_ntop.hpp"
#include "Windows/inet_pton.hpp"
#endif

namespace pona
{

InetAddress::InetAddress(int family, String address, int port)
	: family_(family),
	  socketType_(0),
	  protocol_(0)
{
#ifdef _MSC_VER
	wsaAwareness();
#endif // _MSC_VER

	void* addr = 0;

	if (family == AF_INET)
	{
		inet4Address_.sin_family = AF_INET;
		inet4Address_.sin_port = htons(port);
		inet4Address_.sin_addr.s_addr = htonl(INADDR_ANY);
		addr = &inet4Address_.sin_addr;
	}
	else if (family == AF_INET6)
	{
		inet6Address_.sin6_family = AF_INET6;
		inet6Address_.sin6_port = htons(port);
		inet6Address_.sin6_addr = in6addr_any;
		addr = &inet6Address_.sin6_addr;
	}
	else
		PONA_THROW(NetworkingException, "Unsupported address family");

	if ((address != String()) && ((address != "*"))) {
		char* addressUtf8 = address.strdup();
		if (inet_pton(family, addressUtf8, addr) == -1)
			PONA_THROW(NetworkingException, "Broken address string");
		free(addressUtf8);
	}
}

InetAddress::InetAddress(addrinfo* info)
	: family_(info->ai_family),
	  socketType_(info->ai_socktype),
	  protocol_(info->ai_protocol)
{
#ifdef _MSC_VER
	wsaAwareness();
#endif // _MSC_VER
	
	if (info->ai_canonname)
		canonicalName_ = info->ai_canonname;
	if (info->ai_family == AF_INET)
		inet4Address_ = *(sockaddr_in*)info->ai_addr;
	else if (info->ai_family == AF_INET6)
		inet6Address_ = *(sockaddr_in6*)info->ai_addr;
	else
		PONA_THROW(NetworkingException, "Unsupported address family.");
}

int InetAddress::socketAddressLength() const
{
	int len = 0;
	if (family() == AF_INET)
		len = sizeof(sockaddr_in);
	else if (family() == AF_INET6)
		len = sizeof(sockaddr_in6);
	else
		PONA_THROW(NetworkingException, "Unsupported address family.");
	return len;
}

String InetAddress::familyString() const
{
	String s("UNKNOWN");
	if (family() == AF_INET) s = "INET";
	else if (family() == AF_INET6) s = "INET6";
	else if (family() == AF_UNSPEC) s = "UNSPEC";
	return s;
}

String InetAddress::socketTypeString() const
{
	String s("UNKNOWN");
	if (socketType() == SOCK_DGRAM) s = "DGRAM";
	else if (socketType() == SOCK_STREAM) s = "STREAM";
	return s;
}

String InetAddress::protocolString() const
{
	String s("UNKNOWN");
	if (protocol() == IPPROTO_TCP) s = "TCP";
	else if (protocol() == IPPROTO_UDP) s = "UDP";
	else s = format("<%%>") % protocol();
	return s;
}

String InetAddress::addressString() const
{
	const int bufSize = INET_ADDRSTRLEN + INET6_ADDRSTRLEN;
	char buf[bufSize];
	
	const char* ret = 0;
	const void* addr = 0;
	
	if (socketAddress_.sa_family == AF_INET)
		addr = &inet4Address_.sin_addr;
	else if (socketAddress_.sa_family == AF_INET6)
		addr = &inet6Address_.sin6_addr;
	else
		PONA_THROW(NetworkingException, "Unsupported address family");
	
	ret = inet_ntop(family(), const_cast<void*>(addr), buf, bufSize);
	if (!ret)
		PONA_THROW(NetworkingException, "Illegal binary address format");
	
	return String(buf);
}

int InetAddress::port() const
{
	int port = 0;
	
	if (socketAddress_.sa_family == AF_INET)
		port = inet4Address_.sin_port;
	else if (socketAddress_.sa_family == AF_INET6)
		port = inet6Address_.sin6_port;
	else
		PONA_THROW(NetworkingException, "Unsupported address family");
	
	return ntohs(port);
}

void InetAddress::setPort(int port)
{
	port = htons(port);
	if (socketAddress_.sa_family == AF_INET)
		inet4Address_.sin_port = port;
	else if (socketAddress_.sa_family == AF_INET6)
		inet6Address_.sin6_port = port;
	else
		PONA_THROW(NetworkingException, "Unsupported address family");
}

Ref<InetAddressList, Owner> InetAddress::query( String hostname,
                                                String service,
                                                int family,
                                                int socketType )
{
#ifdef _MSC_VER
	wsaAwareness();
#endif // _MSC_VER

	addrinfo hint;
	addrinfo* head;
	
	memset(&hint, 0, sizeof(hint));
	hint.ai_flags = AI_CANONNAME;
	hint.ai_protocol = family;
	hint.ai_socktype = socketType;
	
	char* hostnameUtf8 = hostname.strdup();
	char* serviceUtf8 = service.strdup();
	
	int ret = getaddrinfo(hostnameUtf8, (service != String()) ? serviceUtf8 : 0, &hint, &head);
	if (ret != 0)
		if (ret != EAI_NONAME)
#ifdef _MSC_VER
			PONA_THROW(NetworkingException, "Failed to read address info");
#else
			PONA_THROW(NetworkingException, gai_strerror(ret));
#endif
	
	free(serviceUtf8);
	free(hostnameUtf8);
	
	Ref<InetAddressList, Owner> list = new InetAddressList;
	
	addrinfo* next = head;
	while (next)
	{
		if ((next->ai_family == AF_INET) || (next->ai_family == AF_INET6))
			list->append(new InetAddress(next));
		next = next->ai_next;
	}

	freeaddrinfo(head);
	
	return list;
}

String InetAddress::hostName()
{
#ifdef _MSC_VER
	wsaAwareness();
#endif // _MSC_VER

	const int bufSize = 1024;
	char buf[bufSize + 1];
	String name;
	if (gethostname(buf, bufSize) != -1)
	{
		buf[bufSize] = 0;
		name = buf;
	}
	return name;
}

} // namespace pona
