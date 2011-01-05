/*
 * ThreadFactory.cpp -- thread creation
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */

#include "ThreadFactory.hpp"

namespace ftl
{

ThreadFactory::ThreadFactory(Ref< Duplicable<Thread> > prototype)
	: prototype_(prototype)
{
	int ret = pthread_attr_init(&attr_);
	if (ret != 0)
		FTL_PTHREAD_EXCEPTION("pthread_attr_init", ret);
}

ThreadFactory::~ThreadFactory()
{
	int ret = pthread_attr_destroy(&attr_);
	if (ret != 0)
		FTL_PTHREAD_EXCEPTION("pthread_attr_destroy", ret);
}

int ThreadFactory::detachState() const
{
	int value = 0;
	int ret = pthread_attr_getdetachstate(&attr_, &value);
	if (ret != 0)
		FTL_PTHREAD_EXCEPTION("pthread_attr_getdetachstate", ret);
	return value;
}

void ThreadFactory::setDetachState(int value)
{
	int ret = pthread_attr_setdetachstate(&attr_, value);
	if (ret != 0)
		FTL_PTHREAD_EXCEPTION("pthread_attr_setdetachstate", ret);
}

size_t ThreadFactory::stackSize() const
{
	size_t value = 0;
	int ret = pthread_attr_getstacksize(&attr_, &value);
	if (ret != 0)
		FTL_PTHREAD_EXCEPTION("pthread_attr_getstacksize", ret);
	return value;
}

void ThreadFactory::setStackSize(size_t value)
{
	int ret = pthread_attr_setstacksize(&attr_, value);
	if (ret != 0)
		FTL_PTHREAD_EXCEPTION("pthread_attr_setstacksize", ret);
}

size_t ThreadFactory::guardSize() const
{
	size_t value = 0;
	int ret = pthread_attr_getguardsize(&attr_, &value);
	if (ret != 0)
		FTL_PTHREAD_EXCEPTION("pthread_attr_getguardsize", ret);
	return value;
}

void ThreadFactory::setGuardSize(size_t value)
{
	int ret = pthread_attr_setguardsize(&attr_, value);
	if (ret != 0)
		FTL_PTHREAD_EXCEPTION("pthread_attr_setguardsize", ret);
}

pthread_attr_t* ThreadFactory::attr() { return &attr_; }

Ref<Thread, Owner> ThreadFactory::produce()
{
	Ref<Thread, Owner> thread = prototype_->duplicate();
	start(thread);
	return thread;
}

void ThreadFactory::start(Ref<Thread> thread)
{
	int ret = pthread_create(&thread->tid_, &attr_, &bootstrap, static_cast<void*>(thread));
	if (ret != 0)
		FTL_PTHREAD_EXCEPTION("pthread_create", ret);
}

void* ThreadFactory::bootstrap(void* self)
{
	Thread* thread = static_cast<Thread*>(self);
	thread->run();
	return (void*)thread;
}

} // namespace ftl
