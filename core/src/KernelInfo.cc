/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <sys/utsname.h>
#include <cc/exceptions>
#include <cc/Singleton>
#include <cc/KernelInfo>

namespace cc {

const KernelInfo *KernelInfo::instance()
{
    return Singleton<KernelInfo>::instance();
}

KernelInfo::KernelInfo()
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
