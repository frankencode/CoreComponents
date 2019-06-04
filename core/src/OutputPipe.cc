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

namespace cc {

Ref<OutputPipe> OutputPipe::create()
{
    return new OutputPipe;
}

Ref<InputPipe> OutputPipe::connectInput() const
{
    return new InputPipe{this};
}

OutputPipe::OutputPipe()
{
    int fd[2] = { 0, 0 };
    if (::pipe(fd) == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);
    masterFd_ = fd[0];
    slaveFd_ = fd[1];
}

} // namespace cc
