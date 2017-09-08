/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <atomic>
#include <cc/Object>

namespace cc {

class MemoryAccounting
{
public:
    static MemoryAccounting *instance();

    inline bool isEnabled() const { return isEnabled_; }

    inline void reset() {
        bytesAllocated_ = 0;
        bytesFreed_ = 0;
    }

    inline void registerAlloc(size_t size) { bytesAllocated_ += size; }
    inline void registerFree(size_t size) { bytesFreed_ += size; }

    inline size_t bytesAllocated() const { return bytesAllocated_; }
    inline size_t bytesFreed() const { return bytesFreed_; }

private:
    MemoryAccounting();
    bool isEnabled_;
    std::atomic<size_t> bytesAllocated_;
    std::atomic<size_t> bytesFreed_;
};

} // namespace cc
