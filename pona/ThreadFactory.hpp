/*
 * ThreadFactory.hpp -- thread creation
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_THREADFACTORY_HPP
#define PONA_THREADFACTORY_HPP

#include "Thread.hpp"

namespace pona
{

class PONA_API ThreadFactory: public Instance
{
public:
	ThreadFactory(Ref<Thread> prototype = 0);
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
	
	Ref<Thread, Owner> prototype_;
	pthread_attr_t attr_;
};

} // namespace pona

#endif // PONA_THREADFACTORY_HPP
