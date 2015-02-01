/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <flux/assert>
#include <flux/exceptions>
#include <flux/SocketPair>

namespace flux {

SocketPair::SocketPair()
{
    int fd[2];
    fd[0] = 0;
    fd[1] = 0;
    if (::socketpair(AF_LOCAL, SOCK_STREAM, 0, fd) == -1)
        FLUX_SYSTEM_DEBUG_ERROR(errno);
    first_ = SystemStream::create(fd[0]);
    second_ = SystemStream::create(fd[1]);
}

SystemStream *SocketPair::at(int i) const
{
    FLUX_ASSERT(0 <= i && i < 2);
    return (i % 2 == 0) ? first_ : second_;
}

} // namespace flux
