/*
 * Condition.cpp -- wait condition
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */

#include "Condition.hpp"

namespace pona
{

Condition::Condition()
{
	int ret = pthread_cond_init(&cond_, 0);
	if (ret != 0)
		PONA_THROW(SystemException, "pthread_cond_init() failed");
}

Condition::~Condition()
{
	int ret = pthread_cond_destroy(&cond_);
	if (ret != 0)
		PONA_THROW(SystemException, "pthread_cond_destroy() failed");
}

/** Enter wait state and atomically unlock provided mutex.
  * The thread will be woken up again and reaquire the mutex atomically
  * if the availability of the condition is signalled (to an arbritary number
  * of threads by signal() or all threads by broadcast()).
  * Note that the first thread scheduled by the OS may invalidate
  * the condition again.
  */
void Condition::wait(Mutex* mutex)
{
	int ret = pthread_cond_wait(&cond_, &mutex->mutex_);
	if (ret != 0)
		PONA_THROW(SystemException, "pthread_cond_wait() failed");
}

/** Same as wait(), but also wakeup if system time reaches 'timeout'.
  * (see also: now()). Returns true if the condition was signalled
  * before 'timeout', else returns false.
  */
bool Condition::waitUntil(Mutex* mutex, Time timeout)
{
	bool success = true;
	struct timespec ts;
	ts.tv_sec = timeout.sec();
	ts.tv_nsec = timeout.nsec();
	int ret = pthread_cond_timedwait(&cond_, &mutex->mutex_, &ts);
	if (ret != 0) {
		if (ret == ETIMEDOUT)
			success = false;
		else
			PONA_THROW(SystemException, "pthread_cond_timedwait() failed");
	}
	return success;
}

/** Wakeup at least one thread blocked in Condition::wait().
  * (A system might wakeup as many threads as CPU's are idle.)
  */
void Condition::signal()
{
	int ret = pthread_cond_signal(&cond_);
	if (ret != 0)
		PONA_THROW(SystemException, "pthread_cond_signal() failed");
}

/** Wakeup all threads blocked in Condition::wait().
  */
void Condition::broadcast()
{
	int ret = pthread_cond_broadcast(&cond_);
	if (ret != 0)
		PONA_THROW(SystemException, "pthread_cond_broadcast() failed");
}

} // namespace pona
