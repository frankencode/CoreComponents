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
#include "atom"

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
	virtual void beginCritical() { mutex_.acquire(); } \
	virtual void endCritical() { mutex_.release(); } \
private: \
	Mutex mutex_;

} // namespace pona

#endif // PONA_MUTEX_HPP
