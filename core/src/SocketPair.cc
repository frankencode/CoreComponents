/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <sys/socket.h> // socketpair
#include <errno.h>
#include <cc/exceptions>
#include <cc/SocketPair>

namespace cc {

Ref<SocketPair> SocketPair::create()
{
    return new SocketPair{};
}

SocketPair::SocketPair()
{
    int fd[2] = { 0, 0 };
    if (::socketpair(AF_LOCAL, SOCK_STREAM|SOCK_CLOEXEC, 0, fd) == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);
    fd_ = fd[0];
    slaveFd_ = fd[1];
}

} // namespace cc
