/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUX_KERNELINFO_H
#define FLUX_KERNELINFO_H

#include <flux/Object>

namespace flux {

class KernelInfoData;

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

} // namespace flux

#endif // FLUX_KERNELINFO_H
