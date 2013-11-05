/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_MUTEX_H
#define FLUX_MUTEX_H

#include <pthread.h>
#include "generics.h"

namespace flux
{

class Condition;
class ScopeGuard;

class Mutex: public Object, public NonCopyable
{
public:
	inline static Ref<Mutex> create() { return new Mutex; }
	~Mutex();

	bool tryAcquire();
	void acquire();
	void release();

protected:
	friend class Condition;
	Mutex();
	pthread_mutex_t mutex_;
};

} // namespace flux

#endif // FLUX_MUTEX_H