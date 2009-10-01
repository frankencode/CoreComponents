/*
 * ThreadFactory.cpp -- thread creation
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */

#include "ThreadFactory.hpp"

namespace pona
{

ThreadFactory::ThreadFactory(Ref<Thread> prototype)
	: prototype_(prototype)
{
	if (pthread_attr_init(&attr_) != 0)
		PONA_THROW(SystemException, "pthread_attr_init() failed");
}

ThreadFactory::~ThreadFactory()
{
	if (pthread_attr_destroy(&attr_) != 0)
		PONA_THROW(SystemException, "pthread_attr_destroy() failed");
}

int ThreadFactory::detachState() const
{
	int value = 0;
	if (pthread_attr_getdetachstate(&attr_, &value) != 0)
		PONA_THROW(SystemException, "pthread_attr_getdetachstate() failed");
	return value;
}

void ThreadFactory::setDetachState(int value)
{
	if (pthread_attr_setdetachstate(&attr_, value) != 0)
		PONA_THROW(SystemException, "pthread_attr_setdetachstate() failed");
}

size_t ThreadFactory::stackSize() const
{
	size_t value = 0;
	if (pthread_attr_getstacksize(&attr_, &value) != 0)
		PONA_THROW(SystemException, "pthread_attr_getstacksize() failed");
	return value;
}

void ThreadFactory::setStackSize(size_t value)
{
	if (pthread_attr_setstacksize(&attr_, value) != 0)
		PONA_THROW(SystemException, "pthread_attr_setstacksize() failed");
}

size_t ThreadFactory::guardSize() const
{
	size_t value = 0;
	if (pthread_attr_getguardsize(&attr_, &value) != 0)
		PONA_THROW(SystemException, "pthread_attr_getguardsize() failed");
	return value;
}

void ThreadFactory::setGuardSize(size_t value)
{
	if (pthread_attr_setguardsize(&attr_, value) != 0)
		PONA_THROW(SystemException, "pthread_attr_setguardsize() failed");
}

pthread_attr_t* ThreadFactory::attr() { return &attr_; }

Ref<Thread, Owner> ThreadFactory::produce()
{
	Ref<Thread, Owner> thread = prototype_->clone();
	start(thread);
	return thread;
}

void ThreadFactory::start(Ref<Thread> thread)
{
	if (pthread_create(&thread->tid_, &attr_, &bootstrap, static_cast<void*>(thread)) != 0)
		PONA_THROW(SystemException, "pthread_create() failed");
	thread->keepAlive_ = (thread->refCount() > 0);
	if (thread->keepAlive_)
		thread->incRefCount(); // prevent self destruction while running
}

void* ThreadFactory::bootstrap(void* self)
{
	Thread* thread = static_cast<Thread*>(self);
	thread->init();
	thread->run();
	thread->done();
	if (thread->keepAlive_)
		thread->decRefCount(); // allow self destruction before termination
	return (void*)thread;
}

} // namespace pona
