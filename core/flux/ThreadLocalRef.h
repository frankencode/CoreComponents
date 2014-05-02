/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_THREADLOCALREF_H
#define FLUX_THREADLOCALREF_H

#include <pthread.h>
#include "exceptions.h"

namespace flux
{

/** \internal
  * \brief Thread Local Owner Pointer
  */
template<class T>
class ThreadLocalRef
{
public:
	ThreadLocalRef(T *b = 0) {
		int ret = ::pthread_key_create(&key_, &threadExitEvent);
		if (ret != 0) FLUX_SYSTEM_DEBUG_ERROR(ret);
		if (b) set(b);
	}

	~ThreadLocalRef() {
		set(reinterpret_cast<T *>(0));
		::pthread_key_delete(key_);
	}

	inline ThreadLocalRef &operator=(T *b) {
		set(b);
		return *this;
	}

	inline operator T *() { return get(); }

	inline T *operator->() const {
		T *a = get();
		FLUX_ASSERT2(a, "Null reference");
		return a;
	}

private:
	static void threadExitEvent(void *v) {
		T *a = reinterpret_cast<T*>(v);
		if (a) a->decRefCount();
	}

	inline void set(T *b) {
		T *a = get();
		if (a != b) {
			if (a) a->decRefCount();
			::pthread_setspecific(key_, b);
 			if (b) b->incRefCount();
		}
	}

	inline T *get() const {
		return reinterpret_cast<T*>(::pthread_getspecific(key_));
	}

	pthread_key_t key_;
};

} // namespace flux

#endif // FLUX_THREADLOCALREF_H
