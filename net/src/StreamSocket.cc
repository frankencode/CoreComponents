/*
 * Copyright (C) 2007-2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/net/StreamSocket>
#include <cc/exceptions>
#include <cc/Singleton>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h> // close, select, unlink
#include <fcntl.h> // fcntl
#include <poll.h> // poll
#include <errno.h> // errno
#include <math.h> // modf

namespace cc {
namespace net {

StreamSocket::Instance::Instance(int fd):
    SystemStream::Instance{fd}
{}

StreamSocket::Instance::Instance(const SocketAddress &address):
    address_{address}
{}

void StreamSocket::Instance::connect(StreamSocket &other)
{
    int fd[2] = { 0, 0 };
    if (::socketpair(AF_LOCAL, SOCK_STREAM|SOCK_CLOEXEC, 0, fd) == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);
    if (fd_ != -1) close();
    if (other->fd_ != -1) other->close();
    fd_ = fd[0];
    other->fd_ = fd[1];
}

void StreamSocket::Instance::connect(const SocketAddress &peerAddress)
{
    address_ = peerAddress;
    fd_ = ::socket(+address_->family(), SOCK_STREAM, 0);
    if (fd_ == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);

    int flags = 0;

    if (+address_->family() != AF_LOCAL) {
        flags = ::fcntl(fd_, F_GETFL, 0);
        if (flags == -1)
            CC_SYSTEM_DEBUG_ERROR(errno);
        if (::fcntl(fd_, F_SETFL, flags | O_NONBLOCK) == -1)
            CC_SYSTEM_DEBUG_ERROR(errno);
    }

    int ret = -1;
    do ret = ::connect(fd_, address_->addr(), address_->addrLen());
    while (ret == -1 && errno == EINTR);

    if (ret == -1) {
        if (errno != EINPROGRESS)
            CC_SYSTEM_DEBUG_ERROR(errno);
    }

    connected_ = (ret != -1);

    if (+address_->family() != AF_LOCAL) {
        if (::fcntl(fd_, F_SETFL, flags) == -1)
            CC_SYSTEM_DEBUG_ERROR(errno);
    }
}

void StreamSocket::Instance::connect(const String &hostName, int port)
{
    connect(SocketAddress::resolveHost(hostName, port));
}

void StreamSocket::Instance::listen(const SocketAddress &localAddress, int backlog)
{
    address_ = localAddress;

    if (fd_ != -1) close();

    fd_ = ::socket(+address_->family(), SOCK_STREAM, 0);
    if (fd_ == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);

    if (+address_->family() != AF_LOCAL) {
        int on = 1;
        if (::setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == -1)
            CC_SYSTEM_DEBUG_ERROR(errno);

        #ifdef TCP_FASTOPEN
        if (::setsockopt(sfd, SOL_TCP, TCP_FASTOPEN, &backlog, sizeof(backlog)))
            CC_SYSTEM_DEBUG_ERROR(errno);
        #endif

        if (+address_->family() == AF_INET6) {
            int on = 1;
            if (::setsockopt(fd_, IPPROTO_IPV6, IPV6_V6ONLY, &on, sizeof(on)) == -1)
                CC_SYSTEM_DEBUG_ERROR(errno);
        }
    }
    else {
        ::unlink(address_->toString());
    }

    if (::bind(fd_, address_->addr(), address_->addrLen()) == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);

    if (+address_->family() != AF_LOCAL && address_->port() == 0) {
        socklen_t len = address_->addrLen();
        if (::getsockname(fd_, address_->addr(), &len) == -1)
            CC_SYSTEM_DEBUG_ERROR(errno);
    }

    if (::listen(fd_, backlog) == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);
}

StreamSocket StreamSocket::Instance::accept()
{
    StreamSocket client{SocketAddress{address_->family()}};
    accept(client);
    return client;
}

void StreamSocket::Instance::accept(StreamSocket::Instance *client)
{
    socklen_t len = client->address_->addrLen();
    int fdc = ::accept(fd_, client->address_->addr(), &len);
    if (fdc < 0) CC_SYSTEM_DEBUG_ERROR(errno);

    client->fd_ = fdc;
    client->connected_ = true;
}

SocketAddress StreamSocket::Instance::address() const
{
    return address_;
}

bool StreamSocket::Instance::waitForReady(int timeout)
{
    if (connected_) return true;

    struct pollfd fds;
    fds.fd = fd_;
    fds.events = POLLIN|POLLOUT;

    if (timeout < 0) timeout = -1;
    int ret = -1;
    do ret = ::poll(&fds, 1, timeout);
    while (ret == -1 && errno == EINTR);

    if (ret == -1) CC_SYSTEM_DEBUG_ERROR(errno);
    CC_ASSERT(ret == 0 || ret == 1);

    if (ret == 1) {
        int error = 0;
        socklen_t len = sizeof(error);
        if (::getsockopt(fd_, SOL_SOCKET, SO_ERROR, &error, &len) == -1)
             CC_SYSTEM_DEBUG_ERROR(errno);
        if (error != 0)
            throw NetworkError{error};
        connected_ = true;
    }

    return connected_;
}

void StreamSocket::Instance::setRecvTimeout(double interval)
{
    struct timeval tval;
    double sec = 0;
    tval.tv_usec = modf(interval, &sec) * 1e6;
    tval.tv_sec = sec;
    if (::setsockopt(fd_, SOL_SOCKET, SO_RCVTIMEO, &tval, sizeof(tval)) == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);
}

void StreamSocket::Instance::setSendTimeout(double interval)
{
    struct timeval tval;
    double sec = 0;
    tval.tv_usec = modf(interval, &sec) * 1e6;
    tval.tv_sec = sec;
    if (::setsockopt(fd_, SOL_SOCKET, SO_SNDTIMEO, &tval, sizeof(tval)) == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);
}

}} // namespace cc::net
