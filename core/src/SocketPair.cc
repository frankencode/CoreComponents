/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/SocketPair>
#include <cc/exceptions>
#include <sys/socket.h> // socketpair
#include <errno.h>

namespace cc {

SocketPair::Instance::Instance()
{
    int fd[2] = { 0, 0 };
    if (::socketpair(AF_LOCAL, SOCK_STREAM|SOCK_CLOEXEC, 0, fd) == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);

    fd_ = fd[0];
    slaveFd_ = fd[1];
}

SocketPair::Instance::~Instance()
{
    if (slaveFd_ != -1) ::close(slaveFd_);
}

void SocketPair::Instance::onStart()
{
    ::close(slaveFd_);
    slaveFd_ = -1;
}

} // namespace cc
