/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Mutex>
#include <cc/WaitCondition>

namespace cc {

/** \class Semaphore Semaphore.h cc/Semaphore
  * \ingroup threading
  * \brief Waiting for a limited resource
  */
class Semaphore: public Object
{
public:
    /** Create a new semaphore
      * \param value initial value
      * \return new object instance
      */
    inline static Ref<Semaphore> create(int value = 0) {
        return new Semaphore(value);
    }

    /** Acquire a certain amount of a limited resource
      * \param amount number of items to acquire
      */
    void acquire(int amount = 1);

    /** Release a certain amount of a limited resource
      * \param amount number of items to acquire
      */
    void release(int amount = 1);

    /** Acquire all items of a limited resource
      * \param minAmount minimum number of items to acquire
      * \return number of items acquired
      */
    int acquireAll(int minAmount = 1);

    /** Release items of a limited resource on demand
      * \param maxAmount maximum number of items to release
      * \return number of items released
      */
    int releaseOnDemand(int maxAmount = intMax);

    /** Try to acquire a certain amount of a limited resource
      * \param amount number of items to acquire
      * \return number of items released
      */
    bool tryAcquire(int amount = 1);

    /** Try to acquire a certain amount of a limited resource for a giving timeout
      * \param timeout absolute timestamp in seconds since the begin of Epoch
      * \param amount number of items to acquire
      * \see System::now()
      */
    bool acquireBefore(double timeout, int amount = 1);

private:
    Semaphore(int value);

    Ref<Mutex> mutex_;
    Ref<WaitCondition> notEmpty_;
    int supply_, demand_;
};

} // namespace cc
