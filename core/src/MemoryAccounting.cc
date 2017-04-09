/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <stdlib.h>
#include <cc/MemoryAccounting>

namespace cc {

MemoryAccounting *MemoryAccounting::instance()
{
    static MemoryAccounting instance_;
    return &instance_;
}

MemoryAccounting::MemoryAccounting()
{
    isEnabled_ = (getenv("CC_MEM_ACCOUNTING") != 0);
    bytesAllocated_ = 0;
    bytesFreed_ = 0;
}

} // namespace cc
