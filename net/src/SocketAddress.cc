/*
 * Copyright (C) 2007-2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/net/SocketAddress>

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h> // gethostname
#include <netdb.h> // getaddrinfo, freeaddrinfo, getnameinfo
#include <errno.h>
#include <cc/strings>
#include <cc/Format>
#include <cc/exceptions>

namespace cc {
namespace net {

SocketAddress::Instance::Instance()
{
    addr_.sa_family = AF_UNSPEC;
}

SocketAddress::Instance::Instance(ProtocolFamily family, const String &address, int port)
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
                throw InvalidAddressSyntax{address};
        }
    }
}

SocketAddress::Instance::Instance(int port)
{
    inet4Address_.sin_family = AF_INET;
    inet4Address_.sin_port = htons(port);
    inet4Address_.sin_addr.s_addr = htonl(INADDR_BROADCAST);
}

SocketAddress::Instance::Instance(struct sockaddr_in *addr)
{
    inet4Address_ = *addr;
}

SocketAddress::Instance::Instance(struct sockaddr_in6 *addr)
{
    inet6Address_ = *addr;
}

SocketAddress::Instance::Instance(addrinfo *info):
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

bool SocketAddress::Instance::isValid() const
{
    return family() != ProtocolFamily::Unspecified;
}

ProtocolFamily SocketAddress::Instance::family() const
{
    return static_cast<ProtocolFamily>(addr_.sa_family);
}

SocketType SocketAddress::Instance::socketType() const
{
    return socketType_;
}

InternetProtocol SocketAddress::Instance::protocol() const
{
    return protocol_;
}

int SocketAddress::Instance::port() const
{
    int port = 0;

    if (addr_.sa_family == AF_INET)
        port = inet4Address_.sin_port;
    else if (addr_.sa_family == AF_INET6)
        port = inet6Address_.sin6_port;

    return ntohs(port);
}

void SocketAddress::Instance::setPort(int port)
{
    port = htons(port);
    if (addr_.sa_family == AF_INET)
        inet4Address_.sin_port = port;
    else if (addr_.sa_family == AF_INET6)
        inet6Address_.sin6_port = port;
    else
        CC_DEBUG_ERROR("Unsupported address family");
}

String SocketAddress::Instance::networkAddress() const
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

String SocketAddress::Instance::toString() const
{
    if (family() == ProtocolFamily::Local || port() == 0 || port() == 0xFFFF) return networkAddress();
    Format s;
    if (family() == ProtocolFamily::Internet4)
        s << networkAddress() << ":" << port();
    else if (family() == ProtocolFamily::Internet6)
        s << "[" << networkAddress() << "]:" << port();
    return s;
}

int SocketAddress::Instance::scope() const
{
    return (addr_.sa_family == AF_INET6) ? inet6Address_.sin6_scope_id : 0;
}

void SocketAddress::Instance::setScope(int scope)
{
    if (addr_.sa_family == AF_INET6) inet6Address_.sin6_scope_id = scope;
}

String SocketAddress::Instance::lookupHostName() const
{
    const int hostNameSize = NI_MAXHOST;
    const int serviceNameSize = NI_MAXSERV;
    char hostName[hostNameSize];
    char serviceName[serviceNameSize];
    int flags = NI_NAMEREQD;
    if (+socketType_ == SOCK_DGRAM) flags |= NI_DGRAM;

    int ret = getnameinfo(addr(), addrLen(), hostName, hostNameSize, serviceName, serviceNameSize, flags);
    if (ret != 0) CC_DEBUG_ERROR(gai_strerror(ret));

    return String{hostName};
}

String SocketAddress::Instance::lookupServiceName() const
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

    return String{serviceName};
}

uint64_t SocketAddress::Instance::networkPrefix() const
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

struct sockaddr *SocketAddress::Instance::addr()
{
    return &addr_;
}

const struct sockaddr *SocketAddress::Instance::addr() const
{
    return &addr_;
}

int SocketAddress::Instance::addrLen() const
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

const SocketAddressDetails *SocketAddress::Instance::details() const
{
    return Object::cast<const SocketAddressDetails *>(this);
}

bool SocketAddress::Instance::equals(const SocketAddress::Instance *other) const
{
    if (family() != other->family()) return false;

    if (family() == ProtocolFamily::Internet4) {
        return inet4Address_.sin_addr.s_addr == other->inet4Address_.sin_addr.s_addr;
    }
    else if (family() == ProtocolFamily::Internet6) {
        uint8_t const *x = inet6Address_.sin6_addr.s6_addr;
        uint8_t const *y = other->inet6Address_.sin6_addr.s6_addr;
        for (int i = 0; i < 8; ++i) {
            if (x[i] != y[i]) return false;
        }
    }
    else if (family() == ProtocolFamily::Local) {
        return strcmp(localAddress_.sun_path, other->localAddress_.sun_path) == 0;
    }

    return false;
}

ProtocolFamily SocketAddress::guessFamily(const String &address)
{
    ProtocolFamily family = ProtocolFamily::Unspecified;
    if (address->count(':') > 1 || address == "::") family = ProtocolFamily::Internet6;
    else if (address->count('.') > 0 || address == "*") family = ProtocolFamily::Internet4;
    else if (address->count() > 0) family = ProtocolFamily::Local;
    return family;
}

SocketAddress::Instance *SocketAddress::instance()
{
    thread_local Ref<Instance> instance_ { new Instance };
    return instance_;
}

SocketAddress::SocketAddress():
    instance_{SocketAddress::instance()}
{}

SocketAddress SocketAddress::resolveUri(const Uri &uri)
{
    SocketAddress address;

    if (uri->scheme() == "local") {
        address = SocketAddress{ProtocolFamily::Local, uri->requestPath()};
    }
    else {
        if (uri->hostIsNumeric()) {
            address = SocketAddress{uri->family(), uri->host(), uri->port()};
        }
        else {
            auto addressList = SocketAddress::queryConnectionInfo(uri->host());
            if (addressList->count() > 0) {
                address = addressList->at(0);
                address->setPort(uri->port());
            }
            else throw HostNameResolutionError{uri->host()};
        }
    }

    return address;
}

SocketAddress SocketAddress::resolveHost(const String &hostName, int port)
{
    auto addressList = SocketAddress::queryConnectionInfo(hostName);
    if (addressList->count() == 0) throw HostNameResolutionError{hostName};
    SocketAddress address = addressList->at(0);
    if (port) address->setPort(port);
    return address;
}

SocketAddressList SocketAddress::queryConnectionInfo(const String &hostName, const String &serviceName, ProtocolFamily family, SocketType socketType, String *canonicalName)
{
    addrinfo hint;
    addrinfo *head = nullptr;

    memclr(&hint, sizeof(hint));
    if ((hostName == "*") || (hostName == "")) hint.ai_flags |= AI_PASSIVE;
    hint.ai_flags |= (canonicalName && (hint.ai_flags & AI_PASSIVE) == 0) ? AI_CANONNAME : 0;
    hint.ai_family = (hostName == "*") ? AF_INET : +family;
    hint.ai_socktype = +socketType;

    int ret;
    {
        const char *n = nullptr;
        const char *s = nullptr;
        if ((hint.ai_flags & AI_PASSIVE) == 0) n = hostName;
        if (serviceName != "") s = serviceName;
        ret = getaddrinfo(n, s, &hint, &head);
    }

    if (ret != 0 && ret != EAI_NONAME)
        CC_DEBUG_ERROR(gai_strerror(ret));

    SocketAddressList list;

    if (canonicalName) {
        if (head) {
            if (head->ai_canonname)
                *canonicalName = head->ai_canonname;
        }
    }

    addrinfo *next = head;

    while (next) {
        if ((next->ai_family == AF_INET) || (next->ai_family == AF_INET6))
            list->append(SocketAddress{next});
        next = next->ai_next;
    }

    if (head)
        freeaddrinfo(head);

    return list;
}

}} // namespace cc::net
