/****************************************************************************
**
** This file is part of libPONA - The Portable Network Abstraction Library.
**
** Copyright (C) 2007-2009  Frank Mertens
**
** This file is part of a free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License as published
** by the Free Software Foundation, either version 3 of the License,
** or (at your option) any later version.
**
** The library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this libary.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#ifndef PONA_CONDITION_HPP
#define PONA_CONDITION_HPP

#include <stdio.h>
#include <errno.h>

#include "atoms"
#include "Mutex.hpp"

namespace pona
{

class Condition: public Instance
{
public:
	Condition()
	{
		int ret = pthread_cond_init(&cond_, 0);
		if (ret != 0)
			PONA_THROW(SystemException, "pthread_cond_init() failed");
	}
	
	~Condition()
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
	inline void wait(Mutex* mutex)
	{
		int ret = pthread_cond_wait(&cond_, &mutex->mutex_);
		if (ret != 0)
			PONA_THROW(SystemException, "pthread_cond_wait() failed");
	}
	
	/** Same as wait(), but also wakeup if system time reaches 'timeout'.
	  * (see also: now()). Returns true if the condition was signalled
	  * before 'timeout', else returns false.
	  */
	inline bool waitUntil(Mutex* mutex, TimeStamp timeout)
	{
		bool success = true;
		struct timespec ts;
		ts.tv_sec = timeout.secondsPart();
		ts.tv_nsec = timeout.nanoSecondsPart();
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
	inline void signal()
	{
		int ret = pthread_cond_signal(&cond_);
		if (ret != 0)
			PONA_THROW(SystemException, "pthread_cond_signal() failed");
	}
	
	/** Wakeup all threads blocked in Condition::wait().
	  */
	inline void broadcast()
	{
		int ret = pthread_cond_broadcast(&cond_);
		if (ret != 0)
			PONA_THROW(SystemException, "pthread_cond_broadcast() failed");
	}
	
private:
	pthread_cond_t cond_;
};

} // namespace pona

#endif // PONA_CONDITION_HPP
