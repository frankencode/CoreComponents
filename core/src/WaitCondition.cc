/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <pthread.h>
#include <math.h>
#ifndef NDEBUG
#include <cc/check>
#endif
#include <cc/exceptions>
#include <cc/WaitCondition>

namespace cc {

WaitCondition::Instance::Instance()
{
    int ret = pthread_cond_init(&cond_, nullptr);
    if (ret != 0) CC_SYSTEM_DEBUG_ERROR(ret);
}

WaitCondition::Instance::~Instance()
{
    #ifndef NDEBUG
    int ret =
    #endif
    pthread_cond_destroy(&cond_);
    #ifndef NDEBUG
    check(ret == 0);
    #endif
}

void WaitCondition::Instance::wait(Mutex &mutex)
{
    int ret = -1;
    while (true) {
        ret = pthread_cond_wait(&cond_, &mutex->mutex_);
        if (ret != EINTR) break;
    }
    if (ret != 0) CC_SYSTEM_DEBUG_ERROR(ret);
}

bool WaitCondition::Instance::waitUntil(double timeout, Mutex &mutex)
{
    bool success = true;
    struct timespec ts;
    double sec = 0;
    ts.tv_nsec = modf(timeout, &sec) * 1e9;
    ts.tv_sec = sec;
    int ret = -1;
    while (true) {
        ret = pthread_cond_timedwait(&cond_, &mutex->mutex_, &ts);
        if (ret != EINTR) break;
    }
    if (ret != 0) {
        if (ret == ETIMEDOUT)
            success = false;
        else
            CC_SYSTEM_DEBUG_ERROR(ret);
    }
    return success;
}

void WaitCondition::Instance::signal()
{
    int ret = pthread_cond_signal(&cond_);
    if (ret != 0) CC_SYSTEM_DEBUG_ERROR(ret);
}

void WaitCondition::Instance::broadcast()
{
    int ret = pthread_cond_broadcast(&cond_);
    if (ret != 0) CC_SYSTEM_DEBUG_ERROR(ret);
}

} // namespace cc
