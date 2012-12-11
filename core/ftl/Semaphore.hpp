/*
 * Semaphore.hpp -- synchronising ressource allocation
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */
#ifndef FTL_SEMAPHORE_HPP
#define FTL_SEMAPHORE_HPP

#include "atoms"
#include "Mutex.hpp"
#include "Condition.hpp"

namespace ftl
{

class Semaphore: public Instance
{
public:
	inline static Ref<Semaphore, Owner> create(int value = 0) {
		return new Semaphore(value);
	}

	void acquire(int amount = 1);
	void release(int amount = 1);

	int acquireAll(int minAmount = 1);
	int releaseOnDemand(int maxAmount = intMax);

	bool tryAcquire(int amount = 1);
	bool acquireBefore(Time timeout, int amount = 1);

private:
	Semaphore(int value);

	Ref<Mutex, Owner> mutex_;
	Ref<Condition, Owner> notEmpty_;
	int supply_, demand_;
};

} // namespace ftl

#endif // FTL_SEMAPHORE_HPP
