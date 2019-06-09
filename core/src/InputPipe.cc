/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <fcntl.h> // O_CLOEXEC
#include <unistd.h> // pipe2
#include <errno.h>
#include <cc/exceptions>
#include <cc/OutputPipe>
#include <cc/InputPipe>

namespace cc {

Ref<InputPipe> InputPipe::create()
{
    return new InputPipe;
}

InputPipe::InputPipe()
{
    int fd[2] = { 0, 0 };
    if (::pipe2(fd, O_CLOEXEC) == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);
    fd_ = fd[1];
    slaveFd_ = fd[0];
}

void InputPipe::onStart()
{
    ::close(slaveFd_);
}

InputPipe::InputPipe(const OutputPipe *outputPipe)
{
    slaveFd_ = outputPipe->fd_;
    fd_ = outputPipe->slaveFd_;
}


} // namespace cc
