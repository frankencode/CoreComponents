/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <sys/utsname.h>
#include <flux/exceptions>
#include <flux/KernelInfo>

namespace flux {

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
        FLUX_SYSTEM_DEBUG_ERROR(errno);
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

} // namespace flux
