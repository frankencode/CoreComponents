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
#include "ThreadFactory.hpp"
#include "Thread.hpp"

namespace pona
{

Thread::Thread()
	: keepAlive_(false)
{}

void Thread::start(int detachState)
{
	ThreadFactory factory;
	factory.setDetachState(detachState);
	factory.start(this);
}

void Thread::wait()
{
	int ret = pthread_join(tid_, 0);
	if (ret != 0)
		PONA_THROW(SystemException, "pthread_join() failed");
}

void Thread::kill(int signal)
{
	if (pthread_kill(tid_, signal) != 0)
		PONA_THROW(SystemException, "pthread_kill() failed");
}

void Thread::sleep(Time duration)
{
	Mutex mutex;
	Condition condition;
	mutex.acquire();
	condition.waitUntil(&mutex, now() + duration);
	mutex.release();
}

void Thread::init() {}
void Thread::done() {}

Ref<Instance, Owner> Thread::clone() { return 0; }

} // namespace pona
