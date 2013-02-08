 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */
#ifndef FTL_THREADFACTORY_HPP
#define FTL_THREADFACTORY_HPP

#include "generics.hpp"
#include "Thread.hpp"

namespace ftl
{

class ThreadFactory: public Instance
{
public:
	inline static Ref<ThreadFactory> create(Ref< Clonable<Thread> > prototype = 0) {
		return new ThreadFactory(prototype);
	}
	~ThreadFactory();

	int detachState() const;
	void setDetachState(int value);

	size_t stackSize() const;
	void setStackSize(size_t value);

	size_t guardSize() const;
	void setGuardSize(size_t value);

	pthread_attr_t *attr();

	Ref<Thread> produce();
	void start(Thread *thread);

protected:
	ThreadFactory(Ref< Clonable<Thread> > prototype = 0);

private:
	static void *bootstrap(void *self);

	Ref< Clonable<Thread> > prototype_;
	pthread_attr_t attr_;
};

} // namespace ftl

#endif // FTL_THREADFACTORY_HPP
