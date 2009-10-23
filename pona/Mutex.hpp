/*
 * Mutex.hpp -- non-recursive mutex lock
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_MUTEX_HPP
#define PONA_MUTEX_HPP

#include <pthread.h>
#include "atoms"

namespace pona
{

class Condition;

class PONA_API Mutex: public Instance
{
public:
	Mutex();
	~Mutex();

	bool tryAcquire();
	void acquire();
	void release();

private:
	friend class Condition;
	pthread_mutex_t mutex_;
};

} // namespace pona

#endif // PONA_MUTEX_HPP
