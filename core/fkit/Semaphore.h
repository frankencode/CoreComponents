/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FKIT_SEMAPHORE_H
#define FKIT_SEMAPHORE_H

#include "Mutex.h"
#include "Condition.h"

namespace fkit
{

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

} // namespace fkit

#endif // FKIT_SEMAPHORE_H
