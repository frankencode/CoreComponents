/*
 * Thread.cpp -- threads
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */

#include "strings.hpp"
#include "Exception.hpp"
#include "Time.hpp"
#include "Condition.hpp"
#include "ThreadFactory.hpp"
#include "Thread.hpp"

namespace ftl
{

Ref<Thread, ThreadLocalOwner> Thread::self_;

Ref<Thread> Thread::self()
{
	if (!self_) {
		self_ = new Thread;
		self_->tid_ = pthread_self();
	}
	return self_;
}


void Thread::start()
{
	ThreadFactory::create()->start(this);
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

bool Thread::stillAlive() const
{
	int ret = pthread_kill(tid_, 0);
	if ((ret != 0) && (ret != ESRCH))
		FTL_PTHREAD_EXCEPTION("pthread_kill", ret);
	return (ret == 0);
}

void Thread::sleep(Time duration)
{
	sleepUntil(Time::now() + duration);
}

void Thread::sleepUntil(Time timeout)
{
	Ref<Mutex, Owner> mutex = Mutex::create();
	Ref<Condition, Owner> condition = Condition::create();
	mutex->acquire();
	condition->waitUntil(mutex, timeout);
	mutex->release();
}

void Thread::blockSignals(Ref<SignalSet> set)
{
	int ret = pthread_sigmask(SIG_BLOCK, set, 0/*oset*/);
	if (ret != 0)
		FTL_PTHREAD_EXCEPTION("pthread_sigmask", ret);
}

void Thread::unblockSignals(Ref<SignalSet> set)
{
	int ret = pthread_sigmask(SIG_UNBLOCK, set, 0/*oset*/);
	if (ret != 0)
		FTL_PTHREAD_EXCEPTION("pthread_sigmask", ret);
}

void Thread::run()
{}

void Thread::handleSignal(int signal)
{}

} // namespace ftl
