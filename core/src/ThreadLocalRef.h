/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUX_THREADLOCALREF_H
#define FLUX_THREADLOCALREF_H

#include <pthread.h>
#include <flux/exceptions>

namespace flux {

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
