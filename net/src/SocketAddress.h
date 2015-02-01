/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXNET_SOCKETADDRESS_H
#define FLUXNET_SOCKETADDRESS_H

#include <arpa/inet.h>
#include <netinet/in.h> // sockaddr_in, etc.
#include <netdb.h> // addrinfo
#include <sys/socket.h> // connect
#include <sys/un.h> // sockaddr_un

#include <flux/String>
#include <flux/List>

namespace flux {

class SystemStream;

namespace net {

class SocketAddress;

typedef List< Ref<SocketAddress> > SocketAddressList;

/** \brief Socket address
  */
class SocketAddress: public Object
{
public:
    inline static Ref<SocketAddress> create() {
        return new SocketAddress;
    }

    /** Create a socket address by given protocol family and numerical address.
      * Use the wildcard "*" to address all interfaces of the local host system.
      *
      * \param family protocol family (AF_UNSPEC, AF_INET, AF_INET6 or AF_LOCAL)
      * \param address numerical host address, wildcard ("*") or file path
      * \param port service port
      */
    inline static Ref<SocketAddress> create(int family, String address = String(), int port = 0) {
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

    String networkAddress() const;
    String toString() const;

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
    static Ref<SocketAddressList> resolve(String hostName, String serviceName = String(), int family = AF_UNSPEC, int socketType = 0, String *canonicalName = 0);

    /** Lookup the host name of given address. Usually a reverse DNS
      * lookup will be issued, which may take several seconds.
      */
    String lookupHostName(bool *failed = 0) const;

    /** Lookup the service name. In most setups the service name will be looked up
      * in a local file (/etc/services) and therefore the call returns immediately.
      */
    String lookupServiceName() const;

    /** Returns the network prefix of an IPv6 address (the first 64 bits).
      * For IPv4 addresses the entire address is returned.
      */
    uint64_t networkPrefix() const;

    /** See if this address equals address b
      */
    bool equals(SocketAddress *b) const;

    /** Size of the address in bits
      */
    inline int length() const { return 8 * addrLen(); }

    struct sockaddr *addr();
    const struct sockaddr *addr() const;
    int addrLen() const;

protected:
    SocketAddress();
    SocketAddress(int family, String address, int port);
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

}} // namespace flux::net

#endif // FLUXNET_SOCKETADDRESS_H
