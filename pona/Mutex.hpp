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
	Mutex();
	~Mutex();

	void acquire();
	void release();

private:
	friend class Condition;
	pthread_mutex_t mutex_;
};

#define PONA_SHARED \
public: \
	virtual void acquire() { mutex_.acquire(); } \
	virtual void release() { mutex_.release(); } \
private: \
	Mutex mutex_;

} // namespace pona

#endif // PONA_MUTEX_HPP
