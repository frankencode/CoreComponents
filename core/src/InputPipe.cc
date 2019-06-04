/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <unistd.h> // pipe
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
    if (::pipe(fd) == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);
    masterFd_ = fd[1];
    slaveFd_ = fd[0];
}

InputPipe::InputPipe(const OutputPipe *outputPipe)
{
    slaveFd_ = outputPipe->masterFd_;
    masterFd_ = outputPipe->slaveFd_;
}


} // namespace cc
