/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUX_SEMAPHORE_H
#define FLUX_SEMAPHORE_H

#include <flux/Mutex>
#include <flux/Condition>

namespace flux {

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
	bool acquireBefore(double timeout, int amount = 1);

private:
	Semaphore(int value);

	Ref<Mutex> mutex_;
	Ref<Condition> notEmpty_;
	int supply_, demand_;
};

} // namespace flux

#endif // FLUX_SEMAPHORE_H
