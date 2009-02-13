/****************************************************************************
**
** This file is part of libPONA - The Portable Network Abstractions Library.
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

#include "../Exception.hpp"
#include "../Clock.hpp"
#include "../Thread.hpp"

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
	if (exitType == joinable)
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	else if (exitType == detached)
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	int ret = pthread_create(&tid_, &attr, &runWrapper, (void*)this);
	if (ret != 0)
		PONA_THROW(PosixException, strerror(ret));
	pthread_attr_destroy(&attr);
	incRefCount(); // prevent self destruction while running
}

int Thread::wait()
{
	void* status;
	int ret = pthread_join(tid_, &status);
	if (ret != 0)
		PONA_THROW(PosixException, strerror(ret));
	return (status != 0);
}

void Thread::sleep(TimeStamp dt)
{
	TimeStamp ts = getTime() + dt;
	struct timespec tspec;
	tspec.tv_sec = ts.secondsPart();
	tspec.tv_nsec = ts.nanoSecondsPart();
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	pthread_mutex_init(&mutex, 0);
	pthread_cond_init(&cond, 0);
	pthread_mutex_lock(&mutex);
	pthread_cond_timedwait(&cond, &mutex, &tspec);
	pthread_mutex_unlock(&mutex);
	pthread_cond_destroy(&cond);
	pthread_mutex_destroy(&mutex);
}

void* Thread::runWrapper(void* p)
{
	pthread_setcancelstate(PTHREAD_CANCEL_ASYNCHRONOUS, 0);
	Thread* thread = (Thread*)p;
	int ret = thread->run();
	thread->decRefCount(); // allow self destruction before termination
	return (void*)ret;
}

} // namespace pona
