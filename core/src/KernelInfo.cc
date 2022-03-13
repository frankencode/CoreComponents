/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/KernelInfo>
#include <sys/utsname.h>

namespace cc {

KernelInfo::KernelInfo()
{
    initOnce<State>();
}

KernelInfo::State::State()
{
    struct utsname data;
    if (::uname(&data) == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);

    name = data.sysname;
    release = data.release;
    version = data.version;
    machine = data.machine;
}

} // namespace cc
