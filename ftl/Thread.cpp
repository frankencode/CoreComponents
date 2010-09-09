/*
 * Thread.cpp -- threads
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../COPYING for the license.
 */

#include <sched.h> // sched_yield
#include "strings.hpp"
#include "Exception.hpp"
#include "Time.hpp"
#include "Condition.hpp"
#include "ThreadFactory.hpp"
#include "ThreadLocalOwner.hpp"
#include "Thread.hpp"

namespace ftl
{

Thread::Thread()
	: keepAlive_(false),
	  started_(false)
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
		FTL_PTHREAD_EXCEPTION("pthread_join", ret);
}

void Thread::kill(int signal)
{
	int ret = pthread_kill(tid_, signal);
	if (ret != 0)
		FTL_PTHREAD_EXCEPTION("pthread_kill", ret);
}

/** Return true if the thread has been started else false.
  * This method is only save to be invoked from this thread
  * itself or the thread that has started this thread.
  */
bool Thread::started() const { return started_; }

/** Return true if the thread is running.
  * This method is only save to be invoked from this thread
  * itself or the thread that has started this thread.
  */
bool Thread::isRunning() const
{
	if (!started_) return false;
	int ret = pthread_kill(tid_, 0);
	if ((ret != 0) && (ret != ESRCH))
		FTL_PTHREAD_EXCEPTION("pthread_kill", ret);
	return (ret == 0);
}

void Thread::sleep(Time duration)
{
	sleepUntil(now() + duration);
}

void Thread::sleepUntil(Time timeout)
{
	Mutex mutex;
	Condition condition;
	mutex.acquire();
	condition.waitUntil(&mutex, timeout);
	mutex.release();
}

void Thread::yield()
{
	if (sched_yield() == -1)
		FTL_SYSTEM_EXCEPTION;
}

void Thread::init() {}
void Thread::done() {}

void Thread::signalHandler(int signal) {}

Ref<Thread, ThreadLocalOwner> threadSelf_;

void Thread::enableSignal(int signal)
{
	check(pthread_self() == tid_);
	threadSelf_ = this;
	
	sigset_t mask;
	sigfillset(&mask);
	
	struct sigaction action;
	mem::clr(&action, sizeof(action));
	action.sa_handler = Thread::signalForwarder;
	action.sa_mask = mask;
	if (::sigaction(signal, &action, 0/*oldact*/) == -1)
		FTL_SYSTEM_EXCEPTION;
	
	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, signal);
	int ret = pthread_sigmask(SIG_UNBLOCK, &set, 0/*oset*/);
	if (ret != 0)
		FTL_PTHREAD_EXCEPTION("pthread_sigmask", ret);
}

void Thread::disableSignal(int signal)
{
	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, signal);
	int ret = pthread_sigmask(SIG_BLOCK, &set, 0/*oset*/);
	if (ret != 0)
		FTL_PTHREAD_EXCEPTION("pthread_sigmask", ret);
	
	struct sigaction action;
	mem::clr(&action, sizeof(action));
	action.sa_handler = SIG_DFL;
	if (::sigaction(signal, &action, 0/*oldact*/) == -1)
		FTL_SYSTEM_EXCEPTION;
}

void Thread::signalForwarder(int signal)
{
	if (threadSelf_)
		threadSelf_->signalHandler(signal);
}

} // namespace ftl
