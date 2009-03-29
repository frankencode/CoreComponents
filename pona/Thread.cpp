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

#include "Exception.hpp"
#include "time.hpp"
#include "Mutex.hpp"
#include "Condition.hpp"
#include "Thread.hpp"

namespace pona
{

Thread::Thread()
{}

Thread::~Thread()
{}

void Thread::start(int exitType)
{
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	if (exitType == Joinable)
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	else if (exitType == Detached)
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	int ret = pthread_create(&tid_, &attr, &runWrapper, (void*)this);
	if (ret != 0)
		PONA_THROW(SystemException, "pthread_create() failed");
	pthread_attr_destroy(&attr);
	incRefCount(); // prevent self destruction while running
}

int Thread::wait()
{
	void* status;
	int ret = pthread_join(tid_, &status);
	if (ret != 0)
		PONA_THROW(SystemException, "pthread_join() failed");
	return (status != 0);
}

void Thread::sleep(TimeStamp dt)
{
	Mutex mutex;
	Condition condition;
	mutex.acquire();
	condition.waitUntil(&mutex, now() + dt);
	mutex.release();
}

void* Thread::runWrapper(void* p)
{
	#ifndef NDEBUG
	pthread_setcancelstate(PTHREAD_CANCEL_ASYNCHRONOUS, 0);
	#endif
	Thread* thread = (Thread*)p;
	int ret = thread->run();
	thread->decRefCount(); // allow self destruction before termination
	return (void*)ret;
}

} // namespace pona
