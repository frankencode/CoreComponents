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

Ref<SocketAddress> SocketAddress::create()
{
    return new SocketAddress;
}

Ref<SocketAddress> SocketAddress::create(ProtocolFamily family, const String &address, int port)
{
    return new SocketAddress{family, address, port};
}

Ref<SocketAddress> SocketAddress::createBroadcast(int port)
{
    Ref<SocketAddress> address = new SocketAddress;
    address->inet4Address_.sin_family = AF_INET;
    address->inet4Address_.sin_port = htons(port);
    address->inet4Address_.sin_addr.s_addr = htonl(INADDR_BROADCAST);
    return address;
}

Ref<SocketAddress> SocketAddress::create(struct sockaddr_in *addr)
{
    return new SocketAddress{addr};
}

Ref<SocketAddress> SocketAddress::SocketAddress::create(struct sockaddr_in6 *addr)
{
    return new SocketAddress{addr};
}

Ref<SocketAddress> SocketAddress::create(addrinfo *info)
{
    return new SocketAddress{info};
}

Ref<SocketAddress> SocketAddress::resolve(const Uri *uri)
{
    Ref<SocketAddress> address;

    if (uri->hostIsNumeric()) {
        address = SocketAddress::create(uri->family(), uri->host(), uri->port());
    }
    else {
        Ref<SocketAddressList> addressList = SocketAddress::resolve(uri->host());
        if (addressList->count() > 0) {
            address = addressList->at(0);
            address->setPort(uri->port());
        }
        else throw HostNameResolutionError{Format{"Failed to resolve host name \"%%\""} << uri->host()};
    }

    return address;
}

Ref<SocketAddress> SocketAddress::copy(const SocketAddress *other)
{
    if (!other) return Ref<SocketAddress>();
    return new SocketAddress(other);
}

SocketAddress::SocketAddress():
    socketType_{SocketType::Unspecified},
    protocol_{InternetProtocol::Unspecified}
{
    addr_.sa_family = AF_UNSPEC;
}

SocketAddress::SocketAddress(ProtocolFamily family, const String &address, int port):
    socketType_{SocketType::Unspecified},
    protocol_{InternetProtocol::Unspecified}
{
    void *addr = 0;

    switch (family) {
        case ProtocolFamily::Internet4: {
            // inet4Address_.sin_len = sizeof(addr);
            *(uint8_t *)&inet4Address_ = sizeof(inet4Address_); // uggly, but safe HACK, for BSD4.4
            inet4Address_.sin_family = AF_INET;
            inet4Address_.sin_port = htons(port);
            inet4Address_.sin_addr.s_addr = htonl(INADDR_ANY);
            addr = &inet4Address_.sin_addr;
            break;
        }
        case ProtocolFamily::Internet6: {
            #ifdef SIN6_LEN
            inet6Address_.sin6_len = sizeof(inet6Address_);
            #endif
            inet6Address_.sin6_family = AF_INET6;
            inet6Address_.sin6_port = htons(port);
            inet6Address_.sin6_addr = in6addr_any;
            addr = &inet6Address_.sin6_addr;
            break;
        }
        case ProtocolFamily::Local: {
            localAddress_.sun_family = AF_LOCAL;
            if (unsigned(address->count()) + 1 > sizeof(localAddress_.sun_path))
                CC_DEBUG_ERROR("Socket path exceeds maximum length");
            if (address == "" || address == "*" || address == "::") {
                localAddress_.sun_path[0] = 0;
            }
            else {
                ::memcpy(localAddress_.sun_path, address->chars(), address->count());
                localAddress_.sun_path[address->count()] = 0;
            }
            break;
        }
        case ProtocolFamily::Unspecified: {
            addr_.sa_family = AF_UNSPEC;
            break;
        }
    };

    if (family != ProtocolFamily::Local) {
        if (address != "" && address != "*" && address != "::") {
            if (family == ProtocolFamily::Unspecified) {
                if (address->count(':') >= 2) family = ProtocolFamily::Internet6;
                else family = ProtocolFamily::Internet4;
            }
            if (inet_pton(+family, address, addr) != 1)
                CC_DEBUG_ERROR("Invalid address string");
        }
    }
}

SocketAddress::SocketAddress(struct sockaddr_in *addr):
    inet4Address_{*addr}
{}

SocketAddress::SocketAddress(struct sockaddr_in6 *addr):
    inet6Address_{*addr}
{}

SocketAddress::SocketAddress(addrinfo *info):
    socketType_{static_cast<SocketType>(info->ai_socktype)},
    protocol_{static_cast<InternetProtocol>(info->ai_protocol)}
{
    if (info->ai_family == AF_INET)
        inet4Address_ = *(sockaddr_in *)info->ai_addr;
    else if (info->ai_family == AF_INET6)
        inet6Address_ = *(sockaddr_in6 *)info->ai_addr;
    else
        CC_DEBUG_ERROR("Unsupported address family");
}

SocketAddress::SocketAddress(const SocketAddress *other):
    socketType_{other->socketType_},
    protocol_{other->protocol_}
{
    ::memcpy(addr(), other->addr(), other->addrLen());
}

bool SocketAddress::isValid() const
{
    return family() != ProtocolFamily::Unspecified;
}

ProtocolFamily SocketAddress::family() const { return static_cast<ProtocolFamily>(addr_.sa_family); }
SocketType SocketAddress::socketType() const { return socketType_; }
InternetProtocol SocketAddress::protocol() const { return protocol_; }

int SocketAddress::port() const
{
    int port = 0;

    if (addr_.sa_family == AF_INET)
        port = inet4Address_.sin_port;
    else if (addr_.sa_family == AF_INET6)
        port = inet6Address_.sin6_port;

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
    if (family() == ProtocolFamily::Local || port() == 0 || port() == 0xFFFF) return networkAddress();
    Format s;
    if (family() == ProtocolFamily::Internet4)
        s << networkAddress() << ":" << port();
    else if (family() == ProtocolFamily::Internet6)
        s << "[" << networkAddress() << "]:" << port();
    return s;
}

int SocketAddress::scope() const
{
    return (addr_.sa_family == AF_INET6) ? inet6Address_.sin6_scope_id : 0;
}

void SocketAddress::setScope(int scope)
{
    if (addr_.sa_family == AF_INET6) inet6Address_.sin6_scope_id = scope;
}

Ref<SocketAddressList> SocketAddress::resolve(const String &hostName, const String &serviceName, ProtocolFamily family, SocketType socketType, String *canonicalName)
{
    addrinfo hint;
    addrinfo *head = 0;

    memclr(&hint, sizeof(hint));
    if ((hostName == "*") || (hostName == "")) hint.ai_flags |= AI_PASSIVE;
    hint.ai_flags |= (canonicalName && (hint.ai_flags & AI_PASSIVE) == 0) ? AI_CANONNAME : 0;
    hint.ai_family = (hostName == "*") ? AF_INET : +family;
    hint.ai_socktype = +socketType;

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
    if (+socketType_ == SOCK_DGRAM) flags |= NI_DGRAM;

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

    return String{hostName};
}

String SocketAddress::lookupServiceName() const
{

    const int hostNameSize = NI_MAXHOST;
    const int serviceNameSize = NI_MAXSERV;
    char hostName[hostNameSize];
    char serviceName[serviceNameSize];
    int flags = (+socketType_ == SOCK_DGRAM) ? NI_DGRAM : 0;

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
    if (family() == ProtocolFamily::Internet4) {
        prefix = endianGate(inet4Address_.sin_addr.s_addr);
    }
    else if (family() == ProtocolFamily::Internet6) {
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

    if (family() == ProtocolFamily::Internet4) {
        return inet4Address_.sin_addr.s_addr == b->inet4Address_.sin_addr.s_addr;
    }
    else if (family() == ProtocolFamily::Internet6) {
        uint8_t const *x = inet6Address_.sin6_addr.s6_addr;
        uint8_t const *y = b->inet6Address_.sin6_addr.s6_addr;
        for (int i = 0; i < 8; ++i) {
            if (x[i] != y[i]) return false;
        }
    }
    else if (family() == ProtocolFamily::Local) {
        return strcmp(localAddress_.sun_path, b->localAddress_.sun_path) == 0;
    }

    return false;
}

struct sockaddr *SocketAddress::addr()
{
    return &addr_;
}

const struct sockaddr *SocketAddress::addr() const
{
    return &addr_;
}

int SocketAddress::addrLen() const
{
    int len = 0;
    if (family() == ProtocolFamily::Internet4)
        len = sizeof(sockaddr_in);
    else if (family() == ProtocolFamily::Internet6)
        len = sizeof(sockaddr_in6);
    else if (family() == ProtocolFamily::Local)
        len = sizeof(sockaddr_un);
    else {
        len = sizeof(sockaddr_in);
        if (len < int(sizeof(sockaddr_in6))) len = sizeof(sockaddr_in6);
        if (len < int(sizeof(sockaddr_un))) len = sizeof(sockaddr_un);
    }
    return len;
}

}} // namespace cc::net
