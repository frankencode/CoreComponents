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
#include <cc/Pipe>

namespace cc {

Ref<Pipe> Pipe::create()
{
    return new Pipe;
}

Pipe::Pipe()
{
    int fd[2];
    fd[0] = 0;
    fd[1] = 0;
    if (::pipe(fd) == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);
    masterFd_ = fd[0];
    slaveFd_ = fd[1];
}

} // namespace cc
