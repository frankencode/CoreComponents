/*
 * Semaphore.cpp -- synchronising ressource allocation
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */

#include <assert.h>
#include "Semaphore.hpp"

namespace pona
{

Semaphore::Semaphore(int value)
	: supply_(value),
	  demand_(0)
{
	assert(value >= 0);
}

void Semaphore::acquire(int amount)
{
	Mutex::acquire();
	demand_ += amount;
	while (supply_ < amount)
		notEmpty_.wait(this);
	demand_ -= amount;
	supply_ -= amount;
	Mutex::release();
}

void Semaphore::release(int amount)
{
	if (amount <= 0) return;
	Mutex::acquire();
	supply_ += amount;
	notEmpty_.broadcast();
	Mutex::release();
}

int Semaphore::acquireAll(int minAmount)
{
	Mutex::acquire();
	while (supply_ < minAmount)
		notEmpty_.wait(this);
	int amount = supply_;
	supply_ = 0;
	Mutex::release();
	return amount;
}

int Semaphore::releaseOnDemand(int maxAmount)
{
	Mutex::acquire();
	int amount = demand_;
	if (amount > maxAmount) amount = maxAmount;
	if (amount > 0) {
		supply_ += amount;
		notEmpty_.broadcast();
	}
	Mutex::release();
	return amount;
}

bool Semaphore::tryAcquire(int amount)
{
	bool success = false;
	Mutex::acquire();
	if (supply_ >= amount) {
		supply_ -= amount;
		success = true;
	}
	Mutex::release();
	return success;
}

bool Semaphore::acquireBefore(Time timeout, int amount)
{
	bool success = true;
	Mutex::acquire();
	demand_ += amount;
	while (supply_ < amount) {
		success = notEmpty_.waitUntil(this, timeout);
		if (!success) break;
	}
	demand_ -= amount;
	if (success)
		supply_ -= amount;
	Mutex::release();
	return success;
}

} // namespace pona
