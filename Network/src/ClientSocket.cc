/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/ClientSocket>
#include <cc/SystemError>
#include <fcntl.h>
#include <poll.h>

namespace cc {

struct ClientSocket::State: public StreamSocket::State
{
    explicit State(const SocketAddress &address):
        StreamSocket::State{address}
    {
        int flags = 0;

        if (+address.family() != AF_LOCAL) {
            flags = CC_SYSCALL(::fcntl(fd_, F_GETFL, 0));
            CC_SYSCALL(::fcntl(fd_, F_SETFL, flags | O_NONBLOCK));
        }

        int ret = -1;
        do ret = ::connect(fd_, address.addr(), address.addrLen());
        while (ret == -1 && errno == EINTR);

        if (ret == -1) {
            if (errno != EINPROGRESS)
                CC_SYSTEM_DEBUG_ERROR(errno);
        }

        established_ = (ret != -1);

        if (+address.family() != AF_LOCAL) {
            CC_SYSCALL(::fcntl(fd_, F_SETFL, flags));
        }
    }

    bool waitEstablished(int timeout) override
    {
        if (established_) return true;

        if (wait(IoEvent::Established, timeout)) {
            int error = 0;
            socklen_t len = sizeof(error);
            CC_SYSCALL(::getsockopt(fd_, SOL_SOCKET, SO_ERROR, &error, &len));
            if (error != 0) throw SocketError{error};
            established_ = true;
        }

        return established_;
    }

    bool established_ { false };
};

ClientSocket::ClientSocket(const String &serverName, const String &serviceName):
    ClientSocket{SocketAddress::resolveHostAndServiceName(serverName, serviceName)}
{}

ClientSocket::ClientSocket(const String &serverName, int servicePort):
    ClientSocket{SocketAddress::resolveHostName(serverName, servicePort)}
{}

ClientSocket::ClientSocket(const SocketAddress &serverAddress):
    StreamSocket{new State{serverAddress}}
{}

ClientSocket::State &ClientSocket::me()
{
    return Object::me.as<State>();
}

const ClientSocket::State &ClientSocket::me() const
{
    return Object::me.as<State>();
}

} // namespace cc
