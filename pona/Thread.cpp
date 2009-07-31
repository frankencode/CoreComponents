/*
 * Thread.cpp -- threads
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */

#include "Exception.hpp"
#include "Time.hpp"
#include "Mutex.hpp"
#include "Condition.hpp"
#include "Thread.hpp"

namespace pona
{

Thread::Thread()
	: keepAlive_(false)
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
	if (refCount() != 0) {
		keepAlive_ = true;
		incRefCount(); // prevent self destruction while running
	}
}

int Thread::wait()
{
	int* status;
	int ret = pthread_join(tid_, (void**)&status);
	if (ret != 0)
		PONA_THROW(SystemException, "pthread_join() failed");
	return (status) ? *status : 0;
}

void Thread::sleep(Time duration)
{
	Mutex mutex;
	Condition condition;
	mutex.acquire();
	condition.waitUntil(&mutex, now() + duration);
	mutex.release();
}

pthread_t Thread::tid() const { return tid_; }

void* Thread::runWrapper(void* p)
{
	Thread* thread = (Thread*)p;
	thread->exitCode_ = thread->run();
	if (thread->keepAlive_)
		thread->decRefCount(); // allow self destruction before termination
	return (void*)&thread->exitCode_;
}

} // namespace pona
