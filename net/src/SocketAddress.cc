/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h> // gethostname
#include <netdb.h> // getaddrinfo, freeaddrinfo, getnameinfo
#include <errno.h>
#include <cc/strings>
#include <cc/Format>
#include <cc/exceptions>
#include <cc/SystemStream>
#include <cc/net/Uri>
#include <cc/net/SocketAddress>

namespace cc {
namespace net {

Ref<SocketAddress> SocketAddress::createBroadcast(int port)
{
    Ref<SocketAddress> address = new SocketAddress;
    address->inet4Address_.sin_family = AF_INET;
    address->inet4Address_.sin_port = htons(port);
    address->inet4Address_.sin_addr.s_addr = htonl(INADDR_BROADCAST);
    return address;
}

Ref<SocketAddress> SocketAddress::read(String location)
{
    Ref<SocketAddress> address;
    Ref<Uri> uri;

    try {
        uri = Uri::parse(location);
    }
    catch (UriSyntaxError &ex) {
        throw LocationSyntaxError(Format("Invalid address syntax: \"%%\"") << ex.text());
    }

    if (uri->hostIsNumeric()) {
        address = SocketAddress::create(uri->family(), uri->host(), uri->port());
    }
    else {
        Ref<SocketAddressList> addressList = SocketAddress::resolve(uri->host());
        if (addressList->count() > 0) {
            address = addressList->at(0);
            address->setPort(uri->port());
        }
        else throw HostNameResolutionError(Format("Failed to resolve host name \"%%\"") << uri->host());
    }

    return address;
}

Ref<SocketAddress> SocketAddress::copy(const SocketAddress *other)
{
    if (!other) return Ref<SocketAddress>();
    return new SocketAddress(other);
}

SocketAddress::SocketAddress():
    socketType_(0),
    protocol_(0)
{
    addr_.sa_family = AF_UNSPEC;
}

SocketAddress::SocketAddress(int family, String address, int port):
    socketType_(0),
    protocol_(0)
{
    void *addr = 0;

    if (family == AF_INET) {
        // inet4Address_.sin_len = sizeof(addr);
        *(uint8_t *)&inet4Address_ = sizeof(inet4Address_); // uggly, but safe HACK, for BSD4.4
        inet4Address_.sin_family = AF_INET;
        inet4Address_.sin_port = htons(port);
        inet4Address_.sin_addr.s_addr = htonl(INADDR_ANY);
        addr = &inet4Address_.sin_addr;
    }
    else if (family == AF_INET6) {
        #ifdef SIN6_LEN
        inet6Address_.sin6_len = sizeof(inet6Address_);
        #endif
        inet6Address_.sin6_family = AF_INET6;
        inet6Address_.sin6_port = htons(port);
        inet6Address_.sin6_addr = in6addr_any;
        addr = &inet6Address_.sin6_addr;
    }
    else if (family == AF_LOCAL) {
        localAddress_.sun_family = AF_LOCAL;
        if (unsigned(address->count()) + 1 > sizeof(localAddress_.sun_path))
            CC_DEBUG_ERROR("Socket path exceeds maximum length");
        if (address == "" || address == "*" || address == "::")
            localAddress_.sun_path[0] = 0;
        else
            memcpy(localAddress_.sun_path, address->chars(), address->count() + 1);
    }
    else if (family == AF_UNSPEC) {
        addr_.sa_family = AF_UNSPEC;
    }
    else
        CC_DEBUG_ERROR("Unsupported address family");

    if (family != AF_LOCAL) {
        if (address != "" && address != "*" && address != "::") {
            if (family == AF_UNSPEC) {
                if (address->count(':') >= 2) family = AF_INET6;
                else family = AF_INET;
            }
            if (inet_pton(family, address, addr) != 1)
                CC_DEBUG_ERROR("Invalid address string");
        }
    }
}

SocketAddress::SocketAddress(struct sockaddr_in *addr):
    inet4Address_(*addr)
{}

SocketAddress::SocketAddress(struct sockaddr_in6 *addr):
    inet6Address_(*addr)
{}

SocketAddress::SocketAddress(addrinfo *info):
    socketType_(info->ai_socktype),
    protocol_(info->ai_protocol)
{
    if (info->ai_family == AF_INET)
        inet4Address_ = *(sockaddr_in *)info->ai_addr;
    else if (info->ai_family == AF_INET6)
        inet6Address_ = *(sockaddr_in6 *)info->ai_addr;
    else
        CC_DEBUG_ERROR("Unsupported address family");
}

SocketAddress::SocketAddress(const SocketAddress *other):
    socketType_(other->socketType_),
    protocol_(other->protocol_)
{
    ::memcpy(addr(), other->addr(), other->addrLen());
}

bool SocketAddress::isValid() const
{
    return family() != AF_UNSPEC;
}

int SocketAddress::family() const { return addr_.sa_family; }
int SocketAddress::socketType() const { return socketType_; }
int SocketAddress::protocol() const { return protocol_; }

int SocketAddress::port() const
{
    int port = 0;

    if (addr_.sa_family == AF_INET)
        port = inet4Address_.sin_port;
    else if (addr_.sa_family == AF_INET6)
        port = inet6Address_.sin6_port;
    else
        CC_DEBUG_ERROR("Unsupported address family");

    return ntohs(port);
}

void SocketAddress::setPort(int port)
{
    port = htons(port);
    if (addr_.sa_family == AF_INET)
        inet4Address_.sin_port = port;
    else if (addr_.sa_family == AF_INET6)
        inet6Address_.sin6_port = port;
    else
        CC_DEBUG_ERROR("Unsupported address family");
}

String SocketAddress::networkAddress() const
{
    String s;
    if (addr_.sa_family == AF_LOCAL) {
        s = localAddress_.sun_path;
    }
    else {
        const int bufSize = INET_ADDRSTRLEN + INET6_ADDRSTRLEN;
        char buf[bufSize];

        const void *addr = 0;
        const char *sz = 0;

        if (addr_.sa_family == AF_INET)
            addr = &inet4Address_.sin_addr;
        else if (addr_.sa_family == AF_INET6)
            addr = &inet6Address_.sin6_addr;
        else
            CC_DEBUG_ERROR("Unsupported address family");

        sz = inet_ntop(addr_.sa_family, const_cast<void *>(addr), buf, bufSize);
        if (!sz)
            CC_DEBUG_ERROR("Illegal binary address format");

        s = sz;
    }
    return s;
}

String SocketAddress::toString() const
{
    if (family() == AF_LOCAL || port() == 0 || port() == 0xFFFF) return networkAddress();
    Format s;
    if (family() == AF_INET)
        s << networkAddress() << ":" << port();
    else if (family() == AF_INET6)
        s << "[" << networkAddress() << "]:" << port();
    return s;
}

int SocketAddress::scope() const {
    return (addr_.sa_family == AF_INET6) ? inet6Address_.sin6_scope_id : 0;
}
void SocketAddress::setScope(int scope) {
    if (addr_.sa_family == AF_INET6) inet6Address_.sin6_scope_id = scope;
}

Ref<SocketAddressList> SocketAddress::resolve(String hostName, String serviceName, int family, int socketType, String *canonicalName)
{
    addrinfo hint;
    addrinfo *head = 0;

    memclr(&hint, sizeof(hint));
    if ((hostName == "*") || (hostName == "")) hint.ai_flags |= AI_PASSIVE;
    hint.ai_flags |= (canonicalName && (hint.ai_flags & AI_PASSIVE) == 0) ? AI_CANONNAME : 0;
    hint.ai_family = (hostName == "*") ? AF_INET : family;
    hint.ai_socktype = socketType;

    int ret;
    {
        const char *n = 0;
        const char *s = 0;
        if ((hint.ai_flags & AI_PASSIVE) == 0) n = hostName;
        if (serviceName != "") s = serviceName;
        ret = getaddrinfo(n, s, &hint, &head);
    }

    if (ret != 0 && ret != EAI_NONAME)
        CC_DEBUG_ERROR(gai_strerror(ret));

    Ref<SocketAddressList> list = SocketAddressList::create();

    if (canonicalName) {
        if (head) {
            if (head->ai_canonname)
                *canonicalName = head->ai_canonname;
        }
    }

    addrinfo *next = head;

    while (next) {
        if ((next->ai_family == AF_INET) || (next->ai_family == AF_INET6))
            list->append(new SocketAddress(next));
        next = next->ai_next;
    }

    if (head)
        freeaddrinfo(head);

    return list;
}

String SocketAddress::lookupHostName(bool *failed) const
{
    const int hostNameSize = NI_MAXHOST;
    const int serviceNameSize = NI_MAXSERV;
    char hostName[hostNameSize];
    char serviceName[serviceNameSize];
    int flags = NI_NAMEREQD;
    if (socketType_ == SOCK_DGRAM) flags |= NI_DGRAM;

    int ret = getnameinfo(addr(), addrLen(), hostName, hostNameSize, serviceName, serviceNameSize, flags);

    if (ret != 0) {
        if (!failed)
            CC_DEBUG_ERROR(gai_strerror(ret));
        *failed = true;
        hostName[0] = 0;
    }
    else {
        if (failed)
            *failed = false;
    }

    return String(hostName);
}

String SocketAddress::lookupServiceName() const
{

    const int hostNameSize = NI_MAXHOST;
    const int serviceNameSize = NI_MAXSERV;
    char hostName[hostNameSize];
    char serviceName[serviceNameSize];
    int flags = (socketType_ == SOCK_DGRAM) ? NI_DGRAM : 0;

    hostName[0] = 0;
    serviceName[0] = 0;
    int ret = getnameinfo(addr(), addrLen(), hostName, hostNameSize, serviceName, serviceNameSize, flags);

    if (ret != 0) {
    #ifdef __MACH__
        if (port()) // OSX 10.4 HACK
    #endif
        CC_DEBUG_ERROR(gai_strerror(ret));
    }

    return String(serviceName);
}

uint64_t SocketAddress::networkPrefix() const
{
    uint64_t prefix = 0;
    if (family() == AF_INET) {
        prefix = endianGate(inet4Address_.sin_addr.s_addr);
    }
    else if (family() == AF_INET6) {
        uint8_t const *a = inet6Address_.sin6_addr.s6_addr;
        for (int i = 0; i < 8; ++i) {
            prefix <<= 8;
            prefix |= a[i];
        }
    }
    return prefix;
}

bool SocketAddress::equals(const SocketAddress *b) const
{
    if (family() != b->family()) return false;

    if (family() == AF_INET) {
        return inet4Address_.sin_addr.s_addr == b->inet4Address_.sin_addr.s_addr;
    }
    else if (family() == AF_INET6) {
        uint8_t const *x = inet6Address_.sin6_addr.s6_addr;
        uint8_t const *y = b->inet6Address_.sin6_addr.s6_addr;
        for (int i = 0; i < 8; ++i) {
            if (x[i] != y[i]) return false;
        }
    }
    else if (family() == AF_LOCAL) {
        return strcmp(localAddress_.sun_path, b->localAddress_.sun_path) == 0;
    }

    return false;
}

struct sockaddr *SocketAddress::addr() { return &addr_; }
const struct sockaddr *SocketAddress::addr() const { return &addr_; }

int SocketAddress::addrLen() const
{
    int len = 0;
    if (family() == AF_INET)
        len = sizeof(sockaddr_in);
    else if (family() == AF_INET6)
        len = sizeof(sockaddr_in6);
    else if (family() == AF_LOCAL)
        len = sizeof(sockaddr_un);
    else {
        len = sizeof(sockaddr_in);
        if (len < int(sizeof(sockaddr_in6))) len = sizeof(sockaddr_in6);
        if (len < int(sizeof(sockaddr_un))) len = sizeof(sockaddr_un);
    }
    return len;
}

}} // namespace cc::net
