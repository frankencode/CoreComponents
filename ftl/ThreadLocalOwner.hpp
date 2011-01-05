/*
 * ThreadLocalOwner.hpp -- thread local ownership 'Ref' policy
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
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
	
	inline T* get() const {
		return reinterpret_cast<T*>(::pthread_getspecific(key_));
	}
	
	inline void set(T* b)
	{
		T* a = get();
		
		if (a != b)
		{
			if (a) a->decRefCount();
			
			::pthread_setspecific(key_, b);
			
 			if (b) b->incRefCount();
		}
	}
	
private:
	static void threadExitEvent(void* arg)
	{
		T* instance = reinterpret_cast<T*>(arg);
		instance->decRefCount();
	}
	
	pthread_key_t key_;
};

} // namespace ftl

#endif // FTL_THREADLOCALOWNER_HPP
