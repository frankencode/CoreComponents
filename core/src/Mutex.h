/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUX_MUTEX_H
#define FLUX_MUTEX_H

#include <pthread.h>
#include <flux/generics>

namespace flux {

class Condition;
class ScopeGuard;

class Mutex: public Object, public NonCopyable
{
public:
    inline static Ref<Mutex> create() { return new Mutex; }
    ~Mutex();

    bool tryAcquire();
    void acquire();
    void release();

protected:
    friend class Condition;
    Mutex();
    pthread_mutex_t mutex_;
};

} // namespace flux

#endif // FLUX_MUTEX_H
