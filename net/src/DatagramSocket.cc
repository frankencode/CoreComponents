/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/DatagramSocket>
#include <cstring>

namespace cc {

String HostUnreachable::message() const
{
    return "Host unreachable";
}

DatagramSocket::State::State(const SocketAddress &address):
    Socket::State{address.family(), SOCK_DGRAM}
{
    if (address.port() != 0) {
        int on = 1;
        CC_SYSCALL(::setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)));

        if (address.family() == ProtocolFamily::Inet6) {
            int on = 1;
            CC_SYSCALL(::setsockopt(fd_, IPPROTO_IPV6, IPV6_V6ONLY, &on, sizeof(on)));
        }
    }

    CC_SYSCALL(::bind(fd_, address.addr(), address.addrLen()));
}

int DatagramSocket::State::receive(Out<Bytes> message, Out<SocketAddress> address)
{
    struct sockaddr *src = nullptr;
    socklen_t len = 0;
    if (address != None{}) {
        if (!address()) address() = SocketAddress{New{}};
        src = address().addr();
        len = address().addrLen();
    }
    int ret = -1;
    do ret = ::recvfrom(fd_, message(), message().count(), /*flags =*/0, src, &len);
    while (ret == -1 && errno == EINTR);
    if (ret == -1) {
        if (errno == EHOSTUNREACH) throw HostUnreachable{};
        CC_SYSTEM_DEBUG_ERROR(errno);
    }
    return ret;
}

int DatagramSocket::State::send(const Bytes &message, const SocketAddress &address)
{
    const struct sockaddr *dst = nullptr;
    socklen_t len = 0;
    if (address) {
        dst = address.addr();
        len = address.addrLen();
    }
    int ret = -1;
    do ret = ::sendto(fd_, message, message.count(), 0, dst, len);
    while (ret == -1 && errno == EINTR);
    if (ret == -1) {
        if (errno == EHOSTUNREACH) throw HostUnreachable{};
        CC_SYSTEM_DEBUG_ERROR(errno);
    }
    return ret;
}

void DatagramSocket::State::connect(const SocketAddress &address)
{
    int ret = -1;
    do ret = ::connect(fd_, address.addr(), address.addrLen());
    while (ret == -1 && errno == EINTR);
    if (ret == -1) CC_SYSTEM_DEBUG_ERROR(errno);
}

void DatagramSocket::State::broadcasting(bool on)
{
    int value = on;
    CC_SYSCALL(::setsockopt(fd_, SOL_SOCKET, SO_BROADCAST, &value, sizeof(value)));
}

void DatagramSocket::State::join(const SocketAddress &address)
{
    struct group_req req;
    std::memset(&req, 0, sizeof(req));
    std::memcpy(&req.gr_group, address.addr(), address.addrLen());
    CC_SYSCALL(::setsockopt(fd_, address.level(), MCAST_JOIN_GROUP, &req, sizeof(req)));
}

} // namespace cc
