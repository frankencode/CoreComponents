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

#ifdef PONA_POSIX

#include <pthread.h>
#include "Atoms.hpp"
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
			PONA_THROW(PosixException, strerror(ret));
	}
	
	~Condition()
	{
		int ret = pthread_cond_destroy(&cond_);
		if (ret != 0)
			PONA_THROW(PosixException, strerror(ret));
	}
	
	inline void wait(Mutex* mutex)
	{
		int ret = pthread_cond_wait(&cond_, &mutex->mutex_);
		if (ret != 0)
			PONA_THROW(PosixException, strerror(ret));
	}
	
	inline void wakeup()
	{
		int ret = pthread_cond_signal(&cond_);
		if (ret != 0)
			PONA_THROW(PosixException, strerror(ret));
	}
	
	inline void wakeupAll()
	{
		int ret = pthread_cond_broadcast(&cond_);
		if (ret != 0)
			PONA_THROW(PosixException, strerror(ret));
	}
	
private:
	pthread_cond_t cond_;
};

} // namespace pona

#else // PONA_WINDOWS

#include <Windows.h>
#include "Atoms.hpp"
#include "Mutex.hpp"
#include "Thread.hpp"

namespace pona
{

class Condition: public Instance
{
public:
	Condition()
		: front_(0),
		  back_(0)
	{}
	
	~Condition()
	{}
	
	inline void wait(Mutex* mutex)
	{
		Thread::Tls* tls = Thread::tls;

		// append to queue of waiting threads
		Thread::tlsGlobalLock->acquire();
		if (back_ != 0)
			back_->next = tls;
		else
			front_ = back_ = tls;
		tls->next = 0;
		Thread::tlsGlobalLock->release();
		
		// release mutex and wait
		mutex->release();
		tls->wait.acquire();
		
		// require mutex
		mutex->acquire();
	}
	
	inline void wakeup()
	{
		Thread::tlsGlobalLock->acquire();
		if (front_ != 0) {
			front_->wait.release();
			front_ = front_->next;
		}
		if (front_ == 0)
			back_ = 0;
		Thread::tlsGlobalLock->release();
	}
	
	inline void wakeupAll()
	{
		Thread::tlsGlobalLock->acquire();
		while (front_ != 0) {
			front_->wait.release();
			front_ = front_->next;
		}
		back_ = 0;
		Thread::tlsGlobalLock->release();
	}
	
private:
	Thread::Tls* front_;
	Thread::Tls* back_;
};

} // namespace pona

#endif // platform switch

#endif // PONA_CONDITION_HPP
