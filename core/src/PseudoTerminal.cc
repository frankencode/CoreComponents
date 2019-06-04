/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h> // open
#include <stdlib.h> // posix_openpt, grantpt, unlockpt, ptsname
#include <cc/exceptions>
#include <cc/PseudoTerminal>

namespace cc {

Ref<PseudoTerminal> PseudoTerminal::create()
{
    return new PseudoTerminal;
}

PseudoTerminal::PseudoTerminal()
{
    masterFd_ = ::posix_openpt(O_RDWR|O_NOCTTY);
    if (masterFd_ == -1) CC_SYSTEM_DEBUG_ERROR(errno);
    if (::grantpt(masterFd_) == -1) CC_SYSTEM_DEBUG_ERROR(errno);
    if (::unlockpt(masterFd_) == -1) CC_SYSTEM_DEBUG_ERROR(errno);
    slaveFd_ = ::open(::ptsname(masterFd_), O_RDWR|O_NOCTTY);
    if (slaveFd_ == -1) CC_SYSTEM_DEBUG_ERROR(errno);
}

} // namespace cc
