/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Mutex>

#ifndef NDEBUG
#include <cc/check>
#endif
#include <cc/exceptions>

namespace cc {

Mutex::Instance::Instance()
{
    pthread_mutexattr_t *pattr = 0;
    int ret;
    #ifndef NDEBUG
    pthread_mutexattr_t attr;
    pattr = &attr;
    ret = pthread_mutexattr_init(&attr);
    if (ret != 0) CC_SYSTEM_DEBUG_ERROR(ret);
    ret = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
    if (ret != 0) CC_SYSTEM_DEBUG_ERROR(ret);
    #endif
    ret = pthread_mutex_init(&mutex_, pattr);
    if (ret != 0) CC_SYSTEM_DEBUG_ERROR(ret);
    #ifndef NDEBUG
    ret = pthread_mutexattr_destroy(&attr);
    if (ret != 0) CC_SYSTEM_DEBUG_ERROR(ret);
    #endif
}

Mutex::Instance::~Instance()
{
    #ifndef NDEBUG
    int ret =
    #endif
    pthread_mutex_destroy(&mutex_);
    #ifndef NDEBUG
    check(ret == 0);
    #endif
}

bool Mutex::Instance::tryAcquire()
{
    int ret = -1;
    while (true) {
        ret = pthread_mutex_trylock(&mutex_);
        if (ret != EINTR) break;
    }
    if ((ret != 0) && (ret != EBUSY))
        CC_SYSTEM_DEBUG_ERROR(ret);
    return ret != EBUSY;
}

void Mutex::Instance::acquire()
{
    int ret = -1;
    while (true) {
        ret = pthread_mutex_lock(&mutex_);
        if (ret != EINTR) break;
    }
    if (ret != 0) CC_SYSTEM_DEBUG_ERROR(ret);
}

void Mutex::Instance::release()
{
    int ret = pthread_mutex_unlock(&mutex_);
    if (ret != 0) CC_SYSTEM_DEBUG_ERROR(ret);
}

} // namespace cc
