/*
 * ThreadLocalOwner.hpp -- thread local ownership 'Ref' policy
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_THREADLOCALOWNER_HPP
#define PONA_THREADLOCALOWNER_HPP

#include <pthread.h>
#include "Exception.hpp"

namespace pona
{

template<class T>
class ThreadLocalOwner
{
public:
	ThreadLocalOwner()
	{
		int ret = ::pthread_key_create(&key_, &onThreadExit);
		if (ret != 0)
			PONA_SYSTEM_EXCEPTION;
	}
	
	~ThreadLocalOwner()
	{
		int ret = ::pthread_key_delete(key_);
		if (ret != 0)
			PONA_SYSTEM_EXCEPTION;
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
	static void onThreadExit(void* arg)
	{
		T* instance = reinterpret_cast<T*>(arg);
		instance->decRefCount();
	}
	
	pthread_key_t key_;
};

} // namespace pona

#endif // PONA_THREADLOCALOWNER_HPP
