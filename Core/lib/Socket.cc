/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/Socket>
#include <cc/SystemError>
#include <sys/socket.h>
#include <cmath>

namespace cc {

Socket::State::State(ProtocolFamily family, int type):
    IoStream::State{CC_SYSCALL(::socket(+family, type, 0))}
{}

void Socket::setIncomingTimeout(double interval)
{
    struct timeval tval;
    double sec = 0;
    tval.tv_usec = std::modf(interval, &sec) * 1e6;
    tval.tv_sec = sec;
    CC_SYSCALL(::setsockopt(fd(), SOL_SOCKET, SO_RCVTIMEO, &tval, sizeof(tval)));
}

void Socket::setOutgoingTimeout(double interval)
{
    struct timeval tval;
    double sec = 0;
    tval.tv_usec = std::modf(interval, &sec) * 1e6;
    tval.tv_sec = sec;
    CC_SYSCALL(::setsockopt(fd(), SOL_SOCKET, SO_SNDTIMEO, &tval, sizeof(tval)));
}

SocketAddress Socket::getLocalAddress() const
{
    SocketAddress address{New{}};
    socklen_t len = address.addrLen();
    CC_SYSCALL(::getsockname(fd(), address.addr(), &len));
    return address;
}

} // namespace cc
