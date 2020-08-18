/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/OutputPipe>
#include <cc/exceptions>
#include <fcntl.h> // O_CLOEXEC
#include <unistd.h> // pipe, pipe2
#include <errno.h>

namespace cc {

OutputPipe::Instance::Instance()
{
    int fd[2] = { 0, 0 };
    #if 1
    if (::pipe2(fd, O_CLOEXEC) == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);
    #else
    if (::pipe(fd) == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);
    #endif
    fd_ = fd[0];
    slaveFd_ = fd[1];
}

OutputPipe::Instance::~Instance()
{
    if (slaveFd_ != -1) ::close(slaveFd_);
}

void OutputPipe::Instance::onStart()
{
    ::close(slaveFd_);
    slaveFd_ = -1;
}

} // namespace cc
