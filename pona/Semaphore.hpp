/*
 * Semaphore.hpp -- synchronising ressource allocation
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_SEMAPHORE_HPP
#define PONA_SEMAPHORE_HPP

#include "atoms"
#include "Mutex.hpp"
#include "Condition.hpp"

namespace pona
{

class Semaphore: public Mutex
{
public:
	Semaphore(int value = 0);
	
	void acquire(int amount = 1);
	void release(int amount = 1);
	
	int acquireAll(int minAmount = 1);
	int releaseOnDemand(int maxAmount = intMax);
	
	bool tryAcquire(int amount = 1);
	bool acquireBefore(Time timeout, int amount = 1);
	
private:
	Condition notEmpty_;
	int supply_, demand_;
};

} // namespace pona

#endif // PONA_SEMAPHORE_HPP
