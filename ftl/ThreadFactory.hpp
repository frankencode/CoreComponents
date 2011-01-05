/*
 * ThreadFactory.hpp -- thread creation
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */
#ifndef FTL_THREADFACTORY_HPP
#define FTL_THREADFACTORY_HPP

#include "Thread.hpp"

namespace ftl
{

class ThreadFactory: public Instance
{
public:
	ThreadFactory(Ref< Duplicable<Thread> > prototype = 0);
	~ThreadFactory();
	
	int detachState() const;
	void setDetachState(int value);
	
	size_t stackSize() const;
	void setStackSize(size_t value);
	
	size_t guardSize() const;
	void setGuardSize(size_t value);
	
	pthread_attr_t* attr();
	
	Ref<Thread, Owner> produce();
	void start(Ref<Thread> thread);
	
private:
	static void* bootstrap(void* self);
	
	Ref<Duplicable<Thread>, Owner> prototype_;
	pthread_attr_t attr_;
};

} // namespace ftl

#endif // FTL_THREADFACTORY_HPP
