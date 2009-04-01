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

class Mutex: public Instance
{
public:
	Mutex() { pthread_mutex_init(&mutex_, 0); }
	~Mutex() { pthread_mutex_destroy(&mutex_); }

	inline void acquire() { pthread_mutex_lock(&mutex_); }
	inline void release() { pthread_mutex_unlock(&mutex_); }

private:
	friend class Condition;
	pthread_mutex_t mutex_;
};

} // namespace pona

#endif // PONA_MUTEX_HPP
