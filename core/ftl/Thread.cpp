 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */

#include "strings.hpp"
#include "Exception.hpp"
#include "Condition.hpp"
#include "ThreadFactory.hpp"
#include "System.hpp"
#include "Thread.hpp"

namespace ftl
{

thread_local_hook<Thread> Thread::self_;

Thread *Thread::self()
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

void Thread::sleep(double duration)
{
	sleepUntil(System::now() + duration);
}

void Thread::sleepUntil(double timeout)
{
	Ref<Mutex> mutex = Mutex::create();
	Ref<Condition> condition = Condition::create();
	mutex->acquire();
	condition->waitUntil(mutex, timeout);
	mutex->release();
}

void Thread::blockSignals(SignalSet *set)
{
	int ret = pthread_sigmask(SIG_BLOCK, set->rawSet(), 0/*oset*/);
	if (ret != 0)
		FTL_PTHREAD_EXCEPTION("pthread_sigmask", ret);
}

void Thread::unblockSignals(SignalSet *set)
{
	int ret = pthread_sigmask(SIG_UNBLOCK, set->rawSet(), 0/*oset*/);
	if (ret != 0)
		FTL_PTHREAD_EXCEPTION("pthread_sigmask", ret);
}

void Thread::run()
{}

void Thread::handleSignal(int signal)
{}

} // namespace ftl
