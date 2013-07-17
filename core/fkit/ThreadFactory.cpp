/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "ThreadFactory.h"

namespace fkit
{

ThreadFactory::ThreadFactory(Ref< Clonable<Thread> > prototype)
	: prototype_(prototype)
{
	int ret = pthread_attr_init(&attr_);
	if (ret != 0)
		FKIT_PTHREAD_EXCEPTION("pthread_attr_init", ret);
}

ThreadFactory::~ThreadFactory()
{
	int ret = pthread_attr_destroy(&attr_);
	if (ret != 0)
		FKIT_PTHREAD_EXCEPTION("pthread_attr_destroy", ret);
}

int ThreadFactory::detachState() const
{
	int value = 0;
	int ret = pthread_attr_getdetachstate(&attr_, &value);
	if (ret != 0)
		FKIT_PTHREAD_EXCEPTION("pthread_attr_getdetachstate", ret);
	return value;
}

void ThreadFactory::setDetachState(int value)
{
	int ret = pthread_attr_setdetachstate(&attr_, value);
	if (ret != 0)
		FKIT_PTHREAD_EXCEPTION("pthread_attr_setdetachstate", ret);
}

size_t ThreadFactory::stackSize() const
{
	size_t value = 0;
	int ret = pthread_attr_getstacksize(&attr_, &value);
	if (ret != 0)
		FKIT_PTHREAD_EXCEPTION("pthread_attr_getstacksize", ret);
	return value;
}

void ThreadFactory::setStackSize(size_t value)
{
	int ret = pthread_attr_setstacksize(&attr_, value);
	if (ret != 0)
		FKIT_PTHREAD_EXCEPTION("pthread_attr_setstacksize", ret);
}

size_t ThreadFactory::guardSize() const
{
	size_t value = 0;
	int ret = pthread_attr_getguardsize(&attr_, &value);
	if (ret != 0)
		FKIT_PTHREAD_EXCEPTION("pthread_attr_getguardsize", ret);
	return value;
}

void ThreadFactory::setGuardSize(size_t value)
{
	int ret = pthread_attr_setguardsize(&attr_, value);
	if (ret != 0)
		FKIT_PTHREAD_EXCEPTION("pthread_attr_setguardsize", ret);
}

pthread_attr_t *ThreadFactory::attr() { return &attr_; }

Ref<Thread> ThreadFactory::produce()
{
	Ref<Thread> thread = prototype_->clone();
	start(thread);
	return thread;
}

void ThreadFactory::start(Thread *thread)
{
	int ret = pthread_create(&thread->tid_, &attr_, &bootstrap, static_cast<void*>(thread));
	if (ret != 0)
		FKIT_PTHREAD_EXCEPTION("pthread_create", ret);
}

void *ThreadFactory::bootstrap(void *self)
{
	Thread *thread = static_cast<Thread*>(self);
	Thread::self_ = thread;
	thread->run();
	Thread::self_ = 0;
	return (void *)thread;
}

} // namespace fkit
