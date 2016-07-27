/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Object>

namespace cc {

class KernelInfoData;

/** \class KernelInfo KernelInfo.h cc/KernelInfo
  */
class KernelInfo: public Object
{
public:
    static Ref<KernelInfo> query();

    String name() const;
    String release() const;
    String version() const;
    String machine() const;

private:
    KernelInfo();

    Ref<KernelInfoData> data_;
};

} // namespace cc
