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
#include <flux/LocalConnection>

namespace flux {

LocalConnection::LocalConnection()
{
    fd_[0] = 0;
    fd_[1] = 0;
    if (::socketpair(AF_LOCAL, SOCK_STREAM, 0, fd_) == -1)
        FLUX_SYSTEM_DEBUG_ERROR(errno);
}

} // namespace flux
