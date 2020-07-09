/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/InputPipe>
#include <cc/exceptions>
#include <fcntl.h> // O_CLOEXEC
#include <unistd.h> // pipe2
#include <errno.h>

namespace cc {

InputPipe::Instance::Instance()
{
    int fd[2] = { 0, 0 };
    if (::pipe2(fd, O_CLOEXEC) == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);
    fd_ = fd[1];
    slaveFd_ = fd[0];
}

void InputPipe::Instance::onStart()
{
    ::close(slaveFd_);
}

} // namespace cc
