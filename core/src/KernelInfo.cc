/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <sys/utsname.h>
#include <cc/exceptions>
#include <cc/KernelInfo>

namespace cc {

class KernelInfoData: public Object, public utsname {
public:
    inline static Ref<KernelInfoData> create() { return new KernelInfoData; }
};

Ref<KernelInfo> KernelInfo::query()
{
    return new KernelInfo;
}

KernelInfo::KernelInfo():
    data_(KernelInfoData::create())
{
    if (::uname(data_) == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);
}

String KernelInfo::name() const
{
    return data_->sysname;
}

String KernelInfo::release() const
{
    return data_->release;
}

String KernelInfo::version() const
{
    return data_->version;
}

String KernelInfo::machine() const
{
    return data_->machine;
}

} // namespace cc
