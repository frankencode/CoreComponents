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
#include <unistd.h> // close
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
    fd_ = ::posix_openpt(O_RDWR|O_NOCTTY);
    if (fd_ == -1) CC_SYSTEM_DEBUG_ERROR(errno);
    if (::grantpt(fd_) == -1) CC_SYSTEM_DEBUG_ERROR(errno);
    if (::unlockpt(fd_) == -1) CC_SYSTEM_DEBUG_ERROR(errno);
    slaveFd_ = ::open(::ptsname(fd_), O_RDWR|O_NOCTTY);
    if (slaveFd_ == -1) CC_SYSTEM_DEBUG_ERROR(errno);
}

void PseudoTerminal::onStart()
{
    ::close(slaveFd_);
}

} // namespace cc
