/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <pthread.h>
#include <cc/SpinLock>
#include "helgrind.h"

namespace cc {

#ifndef NDEBUG
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
    bool ok = __sync_bool_compare_and_swap(&flag_, 0, 1);
    VALGRIND_HG_MUTEX_LOCK_POST(&flag_);
    return ok;
}

void SpinLock::acquire()
{
    VALGRIND_HG_MUTEX_LOCK_PRE(&flag_, 0);
    while (!__sync_bool_compare_and_swap(&flag_, 0, 1)) yield();
    VALGRIND_HG_MUTEX_LOCK_POST(&flag_);
}

void SpinLock::release()
{
    VALGRIND_HG_MUTEX_UNLOCK_PRE(&flag_);
    __sync_bool_compare_and_swap(&flag_, 1, 0);
    VALGRIND_HG_MUTEX_UNLOCK_POST(&flag_);
}
#endif // ndef NDEBUG

#ifdef NDEBUG
SpinLock::SpinLock():
    flag_{0}
{}

SpinLock::~SpinLock()
{}

bool SpinLock::tryAcquire()
{
    return __sync_bool_compare_and_swap(&flag_, 0, 1);
}

void SpinLock::acquire()
{
    while (!__sync_bool_compare_and_swap(&flag_, 0, 1)) yield();
}

void SpinLock::release()
{
    flag_ = 0;
}
#endif // def NDEBUG

void SpinLock::yield()
{
    pthread_yield();
}

} // namespace cc
