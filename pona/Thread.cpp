/*
 * Thread.cpp -- threads
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */

#include <string.h> // memset
#include <sched.h> // sched_yield
#include "Exception.hpp"
#include "Time.hpp"
#include "Mutex.hpp"
#include "Condition.hpp"
#include "ThreadFactory.hpp"
#include "ThreadLocalOwner.hpp"
#include "Thread.hpp"

namespace pona
{

Thread::Thread()
	: keepAlive_(false),
	  started_(false)
{}

bool Thread::started() const { return started_; }

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
		PONA_PTHREAD_EXCEPTION("pthread_join", ret);
}

void Thread::kill(int signal)
{
	int ret = pthread_kill(tid_, signal);
	if (ret != 0)
		PONA_PTHREAD_EXCEPTION("pthread_kill", ret);
}

bool Thread::isRunning() const
{
	if (!started_) return false;
	int ret = pthread_kill(tid_, 0);
	if ((ret != 0) && (ret != ESRCH))
		PONA_PTHREAD_EXCEPTION("pthread_kill", ret);
	return (ret == 0);
}

void Thread::sleep(Time duration)
{
	Mutex mutex;
	Condition condition;
	mutex.acquire();
	condition.waitUntil(&mutex, now() + duration);
	mutex.release();
}

void Thread::yield()
{
	if (sched_yield() == -1)
		PONA_SYSTEM_EXCEPTION;
}

Ref<Instance, Owner> Thread::clone() { return 0; }

void Thread::init() {}
void Thread::done() {}

void Thread::signalHandler(int signal) {}

Ref<Thread, ThreadLocalOwner> threadSelf_;

void Thread::enableSignal(int signal)
{
	assert(pthread_self() == tid_);
	threadSelf_ = this;
	
	sigset_t mask;
	sigfillset(&mask);
	
	struct sigaction action;
	::memset(&action, 0, sizeof(action));
	action.sa_handler = Thread::signalForwarder;
	action.sa_mask = mask;
	if (::sigaction(signal, &action, 0/*oldact*/) == -1)
		PONA_SYSTEM_EXCEPTION;
	
	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, signal);
	int ret = pthread_sigmask(SIG_UNBLOCK, &set, 0/*oset*/);
	if (ret != 0)
		PONA_PTHREAD_EXCEPTION("pthread_sigmask", ret);
}

void Thread::disableSignal(int signal)
{
	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, signal);
	int ret = pthread_sigmask(SIG_BLOCK, &set, 0/*oset*/);
	if (ret != 0)
		PONA_PTHREAD_EXCEPTION("pthread_sigmask", ret);
	
	struct sigaction action;
	::memset(&action, 0, sizeof(action));
	action.sa_handler = SIG_DFL;
	if (::sigaction(signal, &action, 0/*oldact*/) == -1)
		PONA_SYSTEM_EXCEPTION;
}

void Thread::signalForwarder(int signal)
{
	if (threadSelf_)
		threadSelf_->signalHandler(signal);
}

} // namespace pona
