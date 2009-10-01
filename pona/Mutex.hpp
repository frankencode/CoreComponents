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
	bool tryAcquire();
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

#define PONA_SHAREABLE \
public: \
	virtual void beginCritical() { if (mutex_) mutex_->acquire(); } \
	virtual void endCritical() { if (mutex_) mutex_->release(); } \
	void makeShared() { if (!mutex_) mutex_ = new Mutex; } \
private: \
	Ref<Mutex, Owner> mutex_;

} // namespace pona

#endif // PONA_MUTEX_HPP
