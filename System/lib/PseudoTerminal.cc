/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/PseudoTerminal>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h> // open
#include <unistd.h> // close
#include <stdlib.h> // posix_openpt, grantpt, unlockpt, ptsname

namespace cc {

PseudoTerminal::State::State()
{
    fd_ = ::posix_openpt(O_RDWR|O_NOCTTY);
    if (fd_ == -1) CC_SYSTEM_DEBUG_ERROR(errno);
    if (::grantpt(fd_) == -1) CC_SYSTEM_DEBUG_ERROR(errno);
    if (::unlockpt(fd_) == -1) CC_SYSTEM_DEBUG_ERROR(errno);
    slaveFd_ = ::open(::ptsname(fd_), O_RDWR|O_NOCTTY);
    if (slaveFd_ == -1) CC_SYSTEM_DEBUG_ERROR(errno);
}

PseudoTerminal::State::~State()
{
    if (slaveFd_ != -1) ::close(slaveFd_);
}

void PseudoTerminal::State::onStart()
{
    if (slaveFd_ != -1) {
        ::close(slaveFd_);
        slaveFd_ = -1;
    }
}

PseudoTerminal::PseudoTerminal():
    IoChannel{new PseudoTerminal::State}
{}

} // namespace cc
