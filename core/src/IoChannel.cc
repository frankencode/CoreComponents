/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <unistd.h> // close
#include <cc/IoChannel>

namespace cc {

IoChannel::IoChannel():
    masterFd_{-1},
    slaveFd_{-1}
{}

int IoChannel::masterFd() const
{
    return masterFd_;
}

int IoChannel::slaveFd() const
{
    return slaveFd_;
}

Ref<SystemStream> IoChannel::open() const
{
    ::close(slaveFd_);
    return SystemStream::create(masterFd_);
}

} // namesapce cc
