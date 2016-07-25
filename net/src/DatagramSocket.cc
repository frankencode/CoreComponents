/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include "DatagramSocket.h"

namespace cc {
namespace net {

String HostUnreachable::message() const
{
    return "Remote host unreachable or service not available";
}

Ref<DatagramSocket> DatagramSocket::open(const SocketAddress *address)
{
    return new DatagramSocket(address);
}

void DatagramSocket::connect(Ref<DatagramSocket> *first, Ref<DatagramSocket> *second)
{
    int fd[2];
    fd[0] = 0;
    fd[1] = 0;
    if (::socketpair(AF_LOCAL, SOCK_DGRAM, 0, fd) == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);
    *first = new DatagramSocket(fd[0]);
    *second = new DatagramSocket(fd[1]);
}

DatagramSocket::DatagramSocket(int fd):
    SystemStream(fd),
    addressFamily_(AF_LOCAL)
{}

DatagramSocket::DatagramSocket(const SocketAddress *address):
    addressFamily_(address->family())
{
    fd_ = ::socket(address->family(), SOCK_DGRAM, 0);
    if (fd_ == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);

    if (address->port() != 0) {
        int on = 1;
        if (::setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == -1)
            CC_SYSTEM_DEBUG_ERROR(errno);

        if (address->family() == AF_INET6) {
            int on = 1;
            if (::setsockopt(fd_, IPPROTO_IPV6, IPV6_V6ONLY, &on, sizeof(on)) == -1)
                CC_SYSTEM_DEBUG_ERROR(errno);
        }
    }

    if (::bind(fd_, address->addr(), address->addrLen()) == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);
}

void DatagramSocket::setRecvBufferSize(int newSize)
{
    if (::setsockopt(fd_, SOL_SOCKET, SO_RCVBUF, &newSize, sizeof(newSize)) == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);
}

void DatagramSocket::setSendBufferSize(int newSize)
{
    if (::setsockopt(fd_, SOL_SOCKET, SO_RCVBUF, &newSize, sizeof(newSize)) == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);
}

int DatagramSocket::recvFrom(Ref<SocketAddress> *peerAddress, ByteArray *buffer)
{
    *peerAddress = SocketAddress::create(addressFamily_);
    socklen_t len = (*peerAddress)->addrLen();
    int ret = ::recvfrom(fd_, buffer->bytes(), buffer->count(), 0, (*peerAddress)->addr(), &len);
    if (ret == -1) {
        if (errno == EHOSTUNREACH) throw HostUnreachable();
        CC_SYSTEM_DEBUG_ERROR(errno);
    }
    return ret;
}

int DatagramSocket::sendTo(const SocketAddress *peerAddress, const ByteArray *message)
{
    int ret = ::sendto(fd_, message->bytes(), message->count(), 0, peerAddress->addr(), peerAddress->addrLen());
    if (ret == -1) {
        if (errno == EHOSTUNREACH) throw HostUnreachable();
        CC_SYSTEM_DEBUG_ERROR(errno);
    }
    return ret;
}

void DatagramSocket::connect(const SocketAddress *peerAddress)
{
    if (::connect(fd_, peerAddress->addr(), peerAddress->addrLen()) == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);
}

}} // namespace cc::net
