 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */
#ifndef FTL_TLO_HPP
#define FTL_TLO_HPP

#include <pthread.h>
#include "Exception.hpp"

namespace ftl
{

/** \internal
  * \brief Thread Local Owner Pointer
  */
template<class T>
class thread_local_hook
{
public:
	thread_local_hook(T *b = 0) {
		int ret = ::pthread_key_create(&key_, &threadExitEvent);
		if (ret != 0)
			FTL_PTHREAD_EXCEPTION("pthread_key_create", ret);
		if (b) set(b);
	}

	~thread_local_hook() {
		set(reinterpret_cast<T *>(0));
		::pthread_key_delete(key_);
	}

	inline thread_local_hook &operator=(T *b) {
		set(b);
		return *this;
	}

	inline operator T *() { return get(); }

	inline T *operator->() const {
		T *a = get();
		FTL_ASSERT2(a, "Null reference");
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

} // namespace ftl

#endif // FTL_TLO_HPP
