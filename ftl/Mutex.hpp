/*
 * Mutex.hpp -- non-recursive mutex lock
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */
#ifndef FTL_MUTEX_HPP
#define FTL_MUTEX_HPP

#include <pthread.h>
#include "atoms"

namespace ftl
{

class Mutex: public Instance, public NonCopyable
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

} // namespace ftl

#endif // FTL_MUTEX_HPP
