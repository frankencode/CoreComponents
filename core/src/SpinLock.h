/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

namespace cc {

/** \brief %Thread synchronization primitive: spinning lock
  */
class SpinLock
{
public:
    SpinLock();
    ~SpinLock();
    bool tryAcquire();
    void acquire();
    void release();

private:
    void yield();
    volatile char flag_;
};

#ifdef NDEBUG
inline SpinLock::SpinLock(): flag_(0) {}
inline SpinLock::~SpinLock() {}
inline bool SpinLock::tryAcquire() { return __sync_bool_compare_and_swap(&flag_, 0, 1); }
inline void SpinLock::acquire() { while (!__sync_bool_compare_and_swap(&flag_, 0, 1)) yield(); }
inline void SpinLock::release() { flag_ = 0; }
#endif // def NDEBUG

} // namespace cc
