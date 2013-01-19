 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */
#ifndef FTL_THREADLOCALOWNER_HPP
#define FTL_THREADLOCALOWNER_HPP

#include <pthread.h>
#include "Exception.hpp"

namespace ftl
{

template<class T>
class ThreadLocalOwner
{
public:
	ThreadLocalOwner()
	{
		int ret = ::pthread_key_create(&key_, &threadExitEvent);
		if (ret != 0)
			FTL_PTHREAD_EXCEPTION("pthread_key_create", ret);
	}

	~ThreadLocalOwner()
	{
		::pthread_key_delete(key_);
		// no error handling, because key could have been deleted automatically
	}

	inline T *get() const {
		return reinterpret_cast<T*>(::pthread_getspecific(key_));
	}

	inline void set(T *b)
	{
		T *a = get();

		if (a != b)
		{
			if (a) a->decRefCount();

			::pthread_setspecific(key_, b);

 			if (b) b->incRefCount();
		}
	}

private:
	static void threadExitEvent(void *arg)
	{
		T *a = reinterpret_cast<T*>(arg);
		if (a) a->decRefCount();
	}

	pthread_key_t key_;
};

} // namespace ftl

#endif // FTL_THREADLOCALOWNER_HPP
