/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ServerSocket>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <unistd.h> // unlink

namespace cc {

ServerSocket::State::State(const SocketAddress &address, int backlog):
    StreamSocket::State{address}
{
    if (+address_.family() != AF_LOCAL) {
        int on = 1;
        CC_SYSCALL(::setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)));

        #ifdef TCP_FASTOPEN
        CC_SYSCALL(::setsockopt(fd_, SOL_TCP, TCP_FASTOPEN, &backlog, sizeof(backlog)));
        #else
        #warning "TCP_FASTOPEN undefined?"
        #endif

        if (+address.family() == AF_INET6) {
            int on = 1;
            CC_SYSCALL(::setsockopt(fd_, IPPROTO_IPV6, IPV6_V6ONLY, &on, sizeof(on)));
        }
    }
    else {
        ::unlink(address_.toString());
    }

    CC_SYSCALL(::bind(fd_, address_.addr(), address_.addrLen()));

    if (+address_.family() != AF_LOCAL && address_.port() == 0) {
        socklen_t len = address_.addrLen();
        CC_SYSCALL(::getsockname(fd_, address_.addr(), &len));
    }

    CC_SYSCALL(::listen(fd_, backlog));
}

StreamSocket ServerSocket::State::accept()
{
    SocketAddress peerAddress{address_.family()};
    socklen_t len = peerAddress.addrLen();
    int fdc = CC_SYSCALL(::accept(fd_, peerAddress.addr(), &len));
    return StreamSocket{peerAddress, fdc};
}

} // namespace cc
