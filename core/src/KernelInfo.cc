/*
 * Copyright (C) 2007-2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/KernelInfo>
#include <sys/utsname.h>
#include <cc/exceptions>
#include <cc/Singleton>

namespace cc {

const KernelInfo::Instance *KernelInfo::operator->() const
{
    return Singleton<KernelInfo::Instance>::instance();
}

KernelInfo::Instance::Instance()
{
    struct utsname *data = new utsname;
    if (::uname(data) == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);
    name_ = data->sysname;
    release_ = data->release;
    version_ = data->version;
    machine_ = data->machine;
    delete data;
}

} // namespace cc
