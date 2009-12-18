/*
 * CoreMutex.hpp -- non-recursive mutex lock
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_COREMUTEX_HPP
#define PONA_COREMUTEX_HPP

#include <pthread.h>

namespace pona
{

class Condition;

class CoreMutex
{
public:
	CoreMutex();
	~CoreMutex();

	bool tryAcquire();
	void acquire();
	void release();

private:
	friend class Condition;
	pthread_mutex_t mutex_;
};

} // namespace pona

#endif // PONA_COREMUTEX_HPP
