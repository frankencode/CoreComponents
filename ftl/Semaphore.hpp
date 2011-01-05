/*
 * Semaphore.hpp -- synchronising ressource allocation
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
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
	Semaphore(int value = 0);
	
	void acquire(int amount = 1);
	void release(int amount = 1);
	
	int acquireAll(int minAmount = 1);
	int releaseOnDemand(int maxAmount = intMax);
	
	bool tryAcquire(int amount = 1);
	bool acquireBefore(Time timeout, int amount = 1);
	
private:
	Mutex mutex_;
	Condition notEmpty_;
	int supply_, demand_;
};

} // namespace ftl

#endif // FTL_SEMAPHORE_HPP
