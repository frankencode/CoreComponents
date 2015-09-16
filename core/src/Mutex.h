/*
 * Copyright (C) 2007-2015 Frank Mertens.
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

class WaitCondition;
class ScopeGuard;

/** \brief Thread synchronization primitive: mutual exclusive access
  */
class Mutex: public Object, public NonCopyable
{
public:
    inline static Ref<Mutex> create() { return new Mutex; }
    ~Mutex();

    bool tryAcquire();
    void acquire();
    void release();

protected:
    friend class WaitCondition;
    Mutex();
    pthread_mutex_t mutex_;
};

} // namespace flux

#endif // FLUX_MUTEX_H
