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

class PONA_API Semaphore: public Instance
{
public:
	Semaphore(int avail = 0);
	
	void acquire(int amount = 1);
	void release(int amount = 1);
	
	bool tryAcquire(int amount = 1);
	bool acquireBefore(Time timeout, int amount = 1);

private:
	Mutex mutex_;
	Condition notEmpty_;
	int avail_;
};

} // namespace pona

#endif // PONA_SEMAPHORE_HPP
