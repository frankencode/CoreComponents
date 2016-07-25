/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <pthread.h>
#include <cc/generics>

namespace cc {

class WaitCondition;
class ScopeGuard;

/** \brief %Thread synchronization primitive: mutual exclusive access
  * \see Guard
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

} // namespace cc
