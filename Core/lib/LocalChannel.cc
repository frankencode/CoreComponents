/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/LocalChannel>
#include <sys/socket.h> // socketpair(2)
#include <unistd.h> // close(2)
#include <errno.h>

namespace cc {

LocalChannel::State::State()
{
    int fd[2] = { 0, 0 };
    CC_SYSCALL(::socketpair(AF_LOCAL, SOCK_STREAM|SOCK_CLOEXEC, 0, fd));

    fd_ = fd[0];
    slaveFd_ = fd[1];
}

LocalChannel::State::~State()
{
    if (slaveFd_ != -1) ::close(slaveFd_);
}

void LocalChannel::State::onStart()
{
    if (slaveFd_ != -1) {
        ::close(slaveFd_);
        slaveFd_ = -1;
    }
}

LocalChannel::LocalChannel():
    IoChannel{new LocalChannel::State}
{}

} // namespace cc
