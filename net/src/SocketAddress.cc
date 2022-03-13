/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/SocketAddress>
#include <cc/Socket>
#include <cc/Uri>
#include <cc/Format>
#include <cc/bytes>
#include <cc/exceptions>
#include <netinet/in.h> // sockaddr_in, etc.
#include <sys/socket.h> // connect
#include <sys/un.h> // sockaddr_un
#include <netdb.h> // addrinfo
#include <cassert>
#include <cstring>

namespace cc {

String str(ProtocolFamily family)
{
    String s{"UNKNOWN"};
    if (+family == AF_INET) s = "INET";
    else if (+family == AF_INET6) s = "INET6";
    else if (+family == AF_UNSPEC) s = "UNSPEC";
    return s;
}

String str(SocketType type)
{
    String s{"UNKNOWN"};
    if (+type == SOCK_DGRAM) s = "DGRAM";
    else if (+type == SOCK_STREAM) s = "STREAM";
    return s;
}

String str(InternetProtocol protocol)
{
    String s{"UNKNOWN"};
    if (+protocol == IPPROTO_TCP) s = "TCP";
    else if (+protocol == IPPROTO_UDP) s = "UDP";
    else s = Format{"<%%>"} << +protocol;
    return s;
}

String HostNameResolutionError::message() const
{
    return Format{"Failed to resolve host name \"%%\""} << hostName_;
}

String HostNameLookupError::message() const
{
    return gai_strerror(errorCode_);
}

String ServiceNameLookupError::message() const
{
    return gai_strerror(errorCode_);
}

InvalidAddressSyntax::InvalidAddressSyntax(const String &address):
    address_{address}
{}

String InvalidAddressSyntax::address() const
{
    return address_;
}

String InvalidAddressSyntax::message() const
{
    return Format{"Failed to parse network address \"%%\""} << address_;
}

struct SocketAddress::State: public Object::State
{
    using Access = CopyOnWrite;

    static ProtocolFamily guessFamily(const String &address)
    {
        ProtocolFamily family = ProtocolFamily::Unspec;
        if (address.contains(":/"));
        else if (address.count(':') > 1) family = ProtocolFamily::Inet6;
        else if (address.count('.') > 0 || address == "*") family = ProtocolFamily::Inet4;
        else if (address.count() > 0) family = ProtocolFamily::Local;
        return family;
    }

    State() {
        addr_.sa_family = AF_UNSPEC;
    }

    State(ProtocolFamily family, const String &address, int port = 0)
    {
        void *addr = nullptr;

        switch (family) {
            case ProtocolFamily::Inet4: {
                std::memset(&inet4Address_, 0, sizeof(inet4Address_));
                // inet4Address_.sin_len = sizeof(addr);
                *(uint8_t *)&inet4Address_ = sizeof(inet4Address_); // uggly for BSD4.4
                inet4Address_.sin_family = AF_INET;
                inet4Address_.sin_port = htons(port);
                inet4Address_.sin_addr.s_addr = htonl(INADDR_ANY);
                addr = &inet4Address_.sin_addr;
                break;
            }
            case ProtocolFamily::Inet6: {
                std::memset(&inet6Address_, 0, sizeof(inet6Address_));
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
                assert(unsigned(address.count()) + 1 <= sizeof(localAddress_.sun_path)); // Socket path exceeds maximum length
                if (address == "" || address == "*" || address == "::") {
                    localAddress_.sun_path[0] = 0;
                }
                else {
                    ::memcpy(localAddress_.sun_path, address.items(), address.count());
                    localAddress_.sun_path[address.count()] = 0;
                }
                break;
            }
            case ProtocolFamily::Unspec: {
                addr_.sa_family = AF_UNSPEC;
                break;
            }
        };

        if (family != ProtocolFamily::Local && family != ProtocolFamily::Unspec) {
            if (address != "" && address != "*" && address != "::") {
                if (inet_pton(+family, address, addr) != 1)
                    throw InvalidAddressSyntax{address};
            }
        }
    }

    explicit State(const String &address, int port = 0):
        State{guessFamily(address), address, port}
    {}

    explicit State(int port)
    {
        inet4Address_.sin_family = AF_INET;
        inet4Address_.sin_port = htons(port);
        inet4Address_.sin_addr.s_addr = htonl(INADDR_BROADCAST);
    }

    explicit State(const Socket &socket)
    {
        socklen_t len = addrLen();
        CC_SYSCALL(::getsockname(socket.fd(), addr(), &len));
    }

    explicit State(struct sockaddr_in *addr)
    {
        inet4Address_ = *addr;
    }

    explicit State(struct sockaddr_in6 *addr)
    {
        inet6Address_ = *addr;
    }

    State(addrinfo *info):
        socketType_{static_cast<SocketType>(info->ai_socktype)},
        protocol_{static_cast<InternetProtocol>(info->ai_protocol)}
    {
        if (info->ai_family == AF_INET)
            inet4Address_ = *(sockaddr_in *)info->ai_addr;
        else if (info->ai_family == AF_INET6)
            inet6Address_ = *(sockaddr_in6 *)info->ai_addr;
        else
            assert(false); // Unsupported address family
    }

    ProtocolFamily family() const
    {
        return static_cast<ProtocolFamily>(addr_.sa_family);
    }

    SocketType socketType() const
    {
        return socketType_;
    }

    InternetProtocol protocol() const
    {
        return protocol_;
    }

    int port() const
    {
        int port = 0;

        if (addr_.sa_family == AF_INET)
            port = inet4Address_.sin_port;
        else if (addr_.sa_family == AF_INET6)
            port = inet6Address_.sin6_port;

        return ntohs(port);
    }

    void setPort(int port)
    {
        port = htons(port);
        if (addr_.sa_family == AF_INET)
            inet4Address_.sin_port = port;
        else if (addr_.sa_family == AF_INET6)
            inet6Address_.sin6_port = port;
        else
            assert(false); // Unsupported address family
    }

    String networkAddress() const
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
                assert(false); // Unsupported address family

            sz = inet_ntop(addr_.sa_family, const_cast<void *>(addr), buf, bufSize);
            assert(sz); // Illegal binary address format

            s = sz;
        }
        return s;
    }

    String toString() const
    {
        if (family() == ProtocolFamily::Local || port() == 0 || port() == 0xFFFF) return networkAddress();
        Format s;
        if (family() == ProtocolFamily::Inet4)
            s << networkAddress() << ":" << port();
        else if (family() == ProtocolFamily::Inet6)
            s << "[" << networkAddress() << "]:" << port();
        return s;
    }

    int scope() const
    {
        return (addr_.sa_family == AF_INET6) ? inet6Address_.sin6_scope_id : 0;
    }

    void setScope(int scope)
    {
        if (addr_.sa_family == AF_INET6) inet6Address_.sin6_scope_id = scope;
    }

    String lookupHostName() const
    {
        String buffer = String::allocate(NI_MAXHOST);
        int flags = NI_NAMEREQD;
        if (+socketType_ == SOCK_DGRAM) flags |= NI_DGRAM;
        int ret = getnameinfo(addr(), addrLen(), buffer.chars(), buffer.count(), nullptr, 0, flags);
        if (ret != 0) throw HostNameLookupError{ret};
        buffer.truncate(std::strlen(buffer.chars()));
        return buffer;
    }

    String lookupServiceName() const
    {
        String buffer = String::allocate(NI_MAXSERV);
        int flags = (+socketType_ == SOCK_DGRAM) ? NI_DGRAM : 0;
        int ret = getnameinfo(addr(), addrLen(), nullptr, 0, buffer.chars(), buffer.count(), flags);
        if (ret != 0) throw ServiceNameLookupError{ret};
        buffer.truncate(std::strlen(buffer.chars()));
        return buffer;
    }

    uint64_t networkPrefix() const
    {
        uint64_t prefix = 0;
        if (family() == ProtocolFamily::Inet4) {
            prefix = endianGate(inet4Address_.sin_addr.s_addr);
        }
        else if (family() == ProtocolFamily::Inet6) {
            uint8_t const *a = inet6Address_.sin6_addr.s6_addr;
            for (int i = 0; i < 8; ++i) {
                prefix <<= 8;
                prefix |= a[i];
            }
        }
        return prefix;
    }

    struct sockaddr *addr()
    {
        return &addr_;
    }

    const struct sockaddr *addr() const
    {
        return &addr_;
    }

    int addrLen() const
    {
        int len = 0;
        if (family() == ProtocolFamily::Inet4)
            len = sizeof(sockaddr_in);
        else if (family() == ProtocolFamily::Inet6)
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

    int level() const
    {
        return (family() == ProtocolFamily::Inet4) ? IPPROTO_IP : IPPROTO_IPV6;
    }

    bool operator==(const State &other) const
    {
        if (family() != other.family()) return false;

        if (family() == ProtocolFamily::Inet4) {
            return inet4Address_.sin_addr.s_addr == other.inet4Address_.sin_addr.s_addr;
        }
        else if (family() == ProtocolFamily::Inet6) {
            uint8_t const *x = inet6Address_.sin6_addr.s6_addr;
            uint8_t const *y = other.inet6Address_.sin6_addr.s6_addr;
            for (int i = 0; i < 8; ++i) {
                if (x[i] != y[i]) return false;
            }
            return true;
        }
        else if (family() == ProtocolFamily::Local) {
            return strcmp(localAddress_.sun_path, other.localAddress_.sun_path) == 0;
        }

        return false;
    }

    bool operator<(const State &other) const
    {
        if (family() < other.family()) return true;
        if (other.family() < family()) return false;

        if (family() == ProtocolFamily::Inet4) {
            return inet4Address_.sin_addr.s_addr < other.inet4Address_.sin_addr.s_addr;
        }
        else if (family() == ProtocolFamily::Inet6) {
            uint8_t const *x = inet6Address_.sin6_addr.s6_addr;
            uint8_t const *y = other.inet6Address_.sin6_addr.s6_addr;
            for (int i = 0; i < 8; ++i) {
                if (x[i] < y[i]) return true;
                else if (y[i] < x[i]) return false;
            }
            return false;
        }
        else if (family() == ProtocolFamily::Local) {
            return strcmp(localAddress_.sun_path, other.localAddress_.sun_path) < 0;
        }

        return false;
    }

    union {
        struct sockaddr addr_;
        struct sockaddr_in inet4Address_;
        struct sockaddr_in6 inet6Address_;
        struct sockaddr_un localAddress_;
    };

    SocketType socketType_ { SocketType::Unspec };
    InternetProtocol protocol_ { InternetProtocol::Unspec };
};

SocketAddress SocketAddress::resolveUri(const Uri &uri, int port)
{
    SocketAddress address;

    if (uri.scheme() == "local") {
        address = SocketAddress{ProtocolFamily::Local, uri.requestPath()};
    }
    else {
        if (uri.hostIsNumeric()) {
            address = SocketAddress{familyOf(uri), uri.host(), uri.port() > 0 ? uri.port() : port};
        }
        else {
            auto addressList = SocketAddress::queryConnectionInfo(uri.host());
            if (addressList.count() > 0) {
                address = addressList.at(0);
                address.setPort(uri.port() > 0 ? uri.port() : port);
            }
            else throw HostNameResolutionError{uri.host()};
        }
    }

    return address;
}

SocketAddress SocketAddress::resolveHostName(const String &hostName, int port)
{
    auto addressList = SocketAddress::queryConnectionInfo(hostName);
    if (addressList.count() == 0) throw HostNameResolutionError{hostName};
    SocketAddress address = addressList.at(0);
    if (port) address.setPort(port);
    return address;
}

SocketAddress SocketAddress::resolveHostAndServiceName(const String &hostName, const String &serviceName)
{
    auto addressList = SocketAddress::queryConnectionInfo(hostName, serviceName);
    if (addressList.count() == 0) throw HostNameResolutionError{hostName};
    return addressList.at(0);
}

List<SocketAddress> SocketAddress::queryConnectionInfo(const String &hostName, const String &serviceName, ProtocolFamily family, SocketType socketType, Out<String> canonicalName)
{
    addrinfo hint;
    addrinfo *head = nullptr;

    memset(&hint, 0, sizeof(hint));
    if ((hostName == "*") || (hostName == "")) hint.ai_flags |= AI_PASSIVE;
    hint.ai_flags |= (canonicalName != None{} && (hint.ai_flags & AI_PASSIVE) == 0) ? AI_CANONNAME : 0;
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

    List<SocketAddress> list;

    if (canonicalName != None{}) {
        if (head) {
            if (head->ai_canonname)
                *canonicalName = head->ai_canonname;
        }
    }

    addrinfo *next = head;

    while (next) {
        if ((next->ai_family == AF_INET) || (next->ai_family == AF_INET6))
            list.append(SocketAddress{new State{next}});
        next = next->ai_next;
    }

    if (head)
        freeaddrinfo(head);

    return list;
}

SocketAddress::SocketAddress(New):
    Object{new State}
{}

SocketAddress::SocketAddress(const Uri &uri, int port)
{
    *this = SocketAddress::resolveUri(uri, port);
}

SocketAddress::SocketAddress(const String &address, int port):
    Object{new State{address, port}}
{}

SocketAddress::SocketAddress(ProtocolFamily family, const String &address, int port):
    Object{new State{family, address, port}}
{}

SocketAddress::SocketAddress(int port):
    Object{new State{port}}
{}

SocketAddress::SocketAddress(const Socket &socket):
    Object{new State{socket}}
{}

SocketAddress::SocketAddress(struct sockaddr_in *addr):
    Object{new State{addr}}
{}

SocketAddress::SocketAddress(struct sockaddr_in6 *addr):
    Object{new State{addr}}
{}

SocketAddress::SocketAddress(State *newState):
    Object{newState}
{}

ProtocolFamily SocketAddress::family() const
{
    return me().family();
}

SocketType SocketAddress::socketType() const
{
    return me().socketType();
}

InternetProtocol SocketAddress::protocol() const
{
    return me().protocol();
}

int SocketAddress::port() const
{
    return me().port();
}

void SocketAddress::setPort(int port)
{
    me().setPort(port);
}

String SocketAddress::networkAddress() const
{
    return me().networkAddress();
}

String SocketAddress::toString() const
{
    return me().toString();
}

int SocketAddress::scope() const
{
    return me().scope();
}

void SocketAddress::setScope(int scope)
{
    me().setScope(scope);
}

String SocketAddress::lookupHostName() const
{
    return me().lookupHostName();
}

String SocketAddress::lookupServiceName() const
{
    return me().lookupServiceName();
}

uint64_t SocketAddress::networkPrefix() const
{
    return me().networkPrefix();
}

struct sockaddr *SocketAddress::addr()
{
    return me().addr();
}

const struct sockaddr *SocketAddress::addr() const
{
    return me().addr();
}

int SocketAddress::addrLen() const
{
    return me().addrLen();
}

int SocketAddress::level() const
{
    return me().level();
}

bool SocketAddress::operator==(const SocketAddress &other) const
{
    return me() == other.me();
}

bool SocketAddress::operator<(const SocketAddress &other) const
{
    return me() < other.me();
}

ProtocolFamily SocketAddress::familyOf(const Uri &uri)
{
    if (!uri.hostIsNumeric()) return ProtocolFamily::Unspec;
    return uri.host().contains(':') ? ProtocolFamily::Inet6 : ProtocolFamily::Inet4;
}

SocketAddress::State &SocketAddress::me()
{
    return Object::me.as<State>();
}

const SocketAddress::State &SocketAddress::me() const
{
    return Object::me.as<State>();
}

} // namespace cc
