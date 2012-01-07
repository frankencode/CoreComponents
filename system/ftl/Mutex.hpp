/*
 * Mutex.hpp -- non-recursive mutex lock
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */
#ifndef FTL_MUTEX_HPP
#define FTL_MUTEX_HPP

#include <pthread.h>
#include "atoms"

namespace ftl
{

class Mutex: public Instance, public NonCopyable
{
public:
	Mutex();
	~Mutex();

	bool tryAcquire();
	void acquire();
	void release();

private:
	friend class Condition;
	pthread_mutex_t mutex_;
};

} // namespace ftl

#endif // FTL_MUTEX_HPP
