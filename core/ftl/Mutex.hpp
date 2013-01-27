 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */
#ifndef FTL_MUTEX_HPP
#define FTL_MUTEX_HPP

#include <pthread.h>
#include "atoms"

namespace ftl
{

class Condition;
class ScopeGuard;

class Mutex: public Instance, public NonCopyable
{
public:
	inline static O<Mutex> create() { return new Mutex; }
	~Mutex();

	bool tryAcquire();
	void acquire();
	void release();

protected:
	friend class Condition;
	friend class ScopeGuard;
	Mutex();
	pthread_mutex_t mutex_;
};

} // namespace ftl

#endif // FTL_MUTEX_HPP
