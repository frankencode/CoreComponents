/*
 * Copyright (C) 2007-2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/SpinLock>
#include <sched.h>
#ifdef CC_VALGRIND
#include <valgrind/helgrind.h>
#endif

namespace cc {

#ifdef CC_VALGRIND

SpinLock::SpinLock():
    flag_{0}
{
    VALGRIND_HG_MUTEX_INIT_POST(&flag_, 0);
}

SpinLock::~SpinLock()
{
    VALGRIND_HG_MUTEX_DESTROY_PRE(&flag_);
}

bool SpinLock::tryAcquire()
{
    VALGRIND_HG_MUTEX_LOCK_PRE(&flag_, 1);
    bool gotIt = !flag_.test_and_set(std::memory_order_acquire);
    VALGRIND_HG_MUTEX_LOCK_POST(&flag_);
    return gotIt;
}

void SpinLock::acquire()
{
    VALGRIND_HG_MUTEX_LOCK_PRE(&flag_, 0);
    while (flag_.test_and_set(std::memory_order_acquire)) yield();
    VALGRIND_HG_MUTEX_LOCK_POST(&flag_);
}

void SpinLock::release()
{
    VALGRIND_HG_MUTEX_UNLOCK_PRE(&flag_);
    flag_.clear(std::memory_order_release);
    VALGRIND_HG_MUTEX_UNLOCK_POST(&flag_);
}

#else

SpinLock::SpinLock():
    flag_{0}
{}

SpinLock::~SpinLock()
{}

bool SpinLock::tryAcquire()
{
    return !flag_.test_and_set(std::memory_order_acquire);
}

void SpinLock::acquire()
{
    while (flag_.test_and_set(std::memory_order_acquire)) yield();
}

void SpinLock::release()
{
    flag_.clear(std::memory_order_release);
}
#endif // def CC_VALGRIND

void SpinLock::yield()
{
    sched_yield();
}

} // namespace cc
