/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <pthread.h>
#include <cc/generics>
#include <cc/Guard>

namespace cc {

class WaitCondition;

/** \class Mutex Mutex.h cc/Mutex
  * \ingroup concurrency
  * \brief %Thread synchronization primitive: mutual exclusive access
  * \see Guard
  */
class Mutex: public Object, public NonCopyable
{
public:
    /** \brief Create a new mutex
      * \return new object instance
      */
    inline static Ref<Mutex> create() { return new Mutex; }

    /** Try to lock this mutex
      * \return true if this mutex was not already locked, false otherwise
      */
    bool tryAcquire();

    /// Lock this mutex
    void acquire();

    /// Release this mutex
    void release();

protected:
    friend class WaitCondition;

    Mutex();
    ~Mutex();

private:
    pthread_mutex_t mutex_;
};

} // namespace cc
