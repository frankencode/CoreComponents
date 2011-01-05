/*
 * Condition.cpp -- wait condition
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */

#include <pthread.h>
#include "Condition.hpp"

namespace ftl
{

Condition::Condition()
{
	int ret = pthread_cond_init(&cond_, 0);
	if (ret != 0)
		FTL_PTHREAD_EXCEPTION("pthread_cond_init", ret);
}

Condition::~Condition()
{
	int ret = pthread_cond_destroy(&cond_);
	if (ret != 0)
		FTL_PTHREAD_EXCEPTION("pthread_cond_destroy", ret);
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
	int ret = -1;
	while (true) {
		ret = pthread_cond_wait(&cond_, &mutex->Mutex::mutex_);
		if (ret != EINTR) break;
	}
	if (ret != 0)
		FTL_PTHREAD_EXCEPTION("pthread_cond_wait", ret);
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
	int ret = -1;
	while (true) {
		ret = pthread_cond_timedwait(&cond_, &mutex->Mutex::mutex_, &ts);
		if (ret != EINTR) break;
	}
	if (ret != 0) {
		if (ret == ETIMEDOUT)
			success = false;
		else
			FTL_PTHREAD_EXCEPTION("pthread_cond_timedwait", ret);
	}
	return success;
}

/** Wakeup at least one waiting thread.
  * (A system might wakeup as many threads as CPU's are idle.)
  */
void Condition::signal()
{
	int ret = pthread_cond_signal(&cond_);
	if (ret != 0)
		FTL_PTHREAD_EXCEPTION("pthread_cond_signal", ret);
}

/** Wakeup all waiting threads.
  */
void Condition::broadcast()
{
	int ret = pthread_cond_broadcast(&cond_);
	if (ret != 0)
		FTL_PTHREAD_EXCEPTION("pthread_cond_broadcast", ret);
}

} // namespace ftl
