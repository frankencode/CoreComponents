/*
 * Semaphore.cpp -- synchronising ressource allocation
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */

#include "Semaphore.hpp"

namespace ftl
{

Semaphore::Semaphore(int value)
	: supply_(value),
	  demand_(0)
{
	check(value >= 0);
}

void Semaphore::acquire(int amount)
{
	mutex_.acquire();
	demand_ += amount;
	while (supply_ < amount)
		notEmpty_.wait(&mutex_);
	demand_ -= amount;
	supply_ -= amount;
	mutex_.release();
}

void Semaphore::release(int amount)
{
	if (amount <= 0) return;
	mutex_.acquire();
	supply_ += amount;
	notEmpty_.broadcast();
	mutex_.release();
}

int Semaphore::acquireAll(int minAmount)
{
	mutex_.acquire();
	while (supply_ < minAmount)
		notEmpty_.wait(&mutex_);
	int amount = supply_;
	supply_ = 0;
	mutex_.release();
	return amount;
}

int Semaphore::releaseOnDemand(int maxAmount)
{
	mutex_.acquire();
	int amount = demand_;
	if (amount > maxAmount) amount = maxAmount;
	if (amount > 0) {
		supply_ += amount;
		notEmpty_.broadcast();
	}
	mutex_.release();
	return amount;
}

bool Semaphore::tryAcquire(int amount)
{
	bool success = false;
	mutex_.acquire();
	if (supply_ >= amount) {
		supply_ -= amount;
		success = true;
	}
	mutex_.release();
	return success;
}

bool Semaphore::acquireBefore(Time timeout, int amount)
{
	bool success = true;
	mutex_.acquire();
	demand_ += amount;
	while (supply_ < amount) {
		success = notEmpty_.waitUntil(&mutex_, timeout);
		if (!success) break;
	}
	demand_ -= amount;
	if (success)
		supply_ -= amount;
	mutex_.release();
	return success;
}

} // namespace ftl
