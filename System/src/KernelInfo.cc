/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the Apache License version 2.0
 * (see CoreComponents/LICENSE-Apache-2.0).
 *
 */

#include <cc/KernelInfo>
#include <sys/utsname.h>

namespace cc {

KernelInfo::KernelInfo():
    Singleton{instance<State>()}
{}

KernelInfo::State::State()
{
    struct utsname data;
    if (::uname(&data) == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);

    name_ = data.sysname;
    release_ = data.release;
    version_ = data.version;
    machine_ = data.machine;
}

} // namespace cc
