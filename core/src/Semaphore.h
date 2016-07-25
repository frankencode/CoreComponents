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

/** \brief waiting for a limited resource
  */
class Semaphore: public Object
{
public:
    inline static Ref<Semaphore> create(int value = 0) {
        return new Semaphore(value);
    }

    void acquire(int amount = 1);
    void release(int amount = 1);

    int acquireAll(int minAmount = 1);
    int releaseOnDemand(int maxAmount = intMax);

    bool tryAcquire(int amount = 1);
    bool acquireBefore(double time, int amount = 1);

private:
    Semaphore(int value);

    Ref<Mutex> mutex_;
    Ref<WaitCondition> notEmpty_;
    int supply_, demand_;
};

} // namespace cc
