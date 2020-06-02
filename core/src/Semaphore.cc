/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Semaphore>

namespace cc {

Semaphore::Instance::Instance(int value):
    supply_{value},
    demand_{0}
{
    CC_ASSERT(value >= 0);
}

void Semaphore::Instance::acquire(int amount)
{
    mutex_->acquire();
    demand_ += amount;
    while (supply_ < amount)
        notEmpty_->wait(mutex_);
    demand_ -= amount;
    supply_ -= amount;
    mutex_->release();
}

void Semaphore::Instance::release(int amount)
{
    if (amount <= 0) return;
    mutex_->acquire();
    supply_ += amount;
    notEmpty_->broadcast();
    mutex_->release();
}

int Semaphore::Instance::acquireAll(int minAmount)
{
    mutex_->acquire();
    while (supply_ < minAmount)
        notEmpty_->wait(mutex_);
    int amount = supply_;
    supply_ = 0;
    mutex_->release();
    return amount;
}

int Semaphore::Instance::releaseOnDemand(int maxAmount)
{
    mutex_->acquire();
    int amount = demand_;
    if (amount > maxAmount) amount = maxAmount;
    if (amount > 0) {
        supply_ += amount;
        notEmpty_->broadcast();
    }
    mutex_->release();
    return amount;
}

bool Semaphore::Instance::tryAcquire(int amount)
{
    bool success = false;
    mutex_->acquire();
    if (supply_ >= amount) {
        supply_ -= amount;
        success = true;
    }
    mutex_->release();
    return success;
}

bool Semaphore::Instance::acquireBefore(double timeout, int amount)
{
    bool success = true;
    mutex_->acquire();
    demand_ += amount;
    while (supply_ < amount) {
        success = notEmpty_->waitUntil(timeout, mutex_);
        if (!success) break;
    }
    demand_ -= amount;
    if (success)
        supply_ -= amount;
    mutex_->release();
    return success;
}

int Semaphore::Instance::getCurrentValue() const
{
    Guard<Mutex> guard{mutex_};
    return supply_;
}

} // namespace cc
