/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/WaitCondition>
#include <cassert>
#include <cmath>

namespace cc {

WaitCondition::WaitCondition()
{
    int ret = pthread_cond_init(&cond_, nullptr);
    if (ret != 0) CC_SYSTEM_DEBUG_ERROR(ret);
}

WaitCondition::~WaitCondition()
{
    #ifndef NDEBUG
    int ret =
    #endif
    pthread_cond_destroy(&cond_);
    #ifndef NDEBUG
    assert(ret == 0);
    #endif
}

void WaitCondition::wait(Mutex &mutex)
{
    int ret = -1;
    do {
        ret = pthread_cond_wait(&cond_, &mutex.mutex_);
    } while (ret == EINTR);

    if (ret != 0) CC_SYSTEM_DEBUG_ERROR(ret);
}

bool WaitCondition::waitUntil(double time, Mutex &mutex)
{
    bool success = true;
    struct timespec ts;
    double sec = 0;
    ts.tv_nsec = std::modf(time, &sec) * 1e9;
    ts.tv_sec = sec;
    int ret = -1;
    do {
        ret = pthread_cond_timedwait(&cond_, &mutex.mutex_, &ts);
    } while (ret == EINTR);

    if (ret != 0) {
        if (ret == ETIMEDOUT)
            success = false;
        else
            CC_SYSTEM_DEBUG_ERROR(ret);
    }
    return success;
}

void WaitCondition::signal()
{
    int ret = pthread_cond_signal(&cond_);
    if (ret != 0) CC_SYSTEM_DEBUG_ERROR(ret);
}

void WaitCondition::broadcast()
{
    int ret = pthread_cond_broadcast(&cond_);
    if (ret != 0) CC_SYSTEM_DEBUG_ERROR(ret);
}

} // namespace cc
