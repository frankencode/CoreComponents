/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_THREADFACTORY_H
#define FLUX_THREADFACTORY_H

#include "generics.h"
#include "Thread.h"

namespace flux
{

class ThreadFactory: public Object
{
public:
	inline static Ref<ThreadFactory> create(Ref< Clonable<Thread> > prototype = 0) {
		return new ThreadFactory(prototype);
	}
	~ThreadFactory();

	int stackSize() const;
	void setStackSize(int value);

	int guardSize() const;

	int detachState() const;
	void setDetachState(int value);

	pthread_attr_t *attr();

	Ref<Thread> produce();
	void start(Thread *thread);

protected:
	ThreadFactory(Ref< Clonable<Thread> > prototype = 0);

private:
	friend class ByteArray;

	Ref<ByteArray> allocateStack() const;
	static void freeStack(ByteArray *stack);
	static void *bootstrap(void *self);

	Ref< Clonable<Thread> > prototype_;
	pthread_attr_t attr_;
	int stackSize_;
	int guardSize_;
};

} // namespace flux

#endif // FLUX_THREADFACTORY_H
