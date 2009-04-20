/*
 * Semaphore.cpp -- synchronising ressource allocation
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */

#include "Semaphore.hpp"

namespace pona
{

Semaphore::Semaphore(int avail)
	: avail_(avail)
{}

void Semaphore::acquire(int amount)
{
	mutex_.acquire();
	while (avail_ < amount)
		notEmpty_.wait(&mutex_);
	avail_ -= amount;
	mutex_.release();
}

void Semaphore::release(int amount)
{
	mutex_.acquire();
	avail_ += amount;
	notEmpty_.signal();
	mutex_.release();
}

bool Semaphore::tryAcquire(int amount)
{
	bool success = false;
	mutex_.acquire();
	if (avail_ >= amount) {
		avail_ -= amount;
		success = true;
	}
	mutex_.release();
	return success;
}

bool Semaphore::acquireBefore(Time timeout, int amount)
{
	bool success = true;
	mutex_.acquire();
	while (avail_ < amount) {
		success = notEmpty_.waitUntil(&mutex_, timeout);
		if (!success) break;
	}
	if (success)
		avail_ -= amount;
	mutex_.release();
	return success;
}

} // namespace pona
