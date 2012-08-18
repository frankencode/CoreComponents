/*
 * ThreadFactory.hpp -- thread creation
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */
#ifndef FTL_THREADFACTORY_HPP
#define FTL_THREADFACTORY_HPP

#include "Thread.hpp"

namespace ftl
{

class ThreadFactory: public Instance
{
public:
	inline static Ref<ThreadFactory, Owner> newInstance(Ref< Clonable<Thread> > prototype = 0) {
		return new ThreadFactory(prototype);
	}
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

protected:
	ThreadFactory(Ref< Clonable<Thread> > prototype = 0);

private:
	static void* bootstrap(void* self);

	Ref<Clonable<Thread>, Owner> prototype_;
	pthread_attr_t attr_;
};

} // namespace ftl

#endif // FTL_THREADFACTORY_HPP
