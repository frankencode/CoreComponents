/*
 * SpinMutex.hpp -- actively waiting mutex lock
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../LICENSE for the license.
 */

#ifndef PONA_SPINMUTEX_HPP
#define PONA_SPINMUTEX_HPP

#ifdef __APPLE__

#include <libkern/OSAtomic.h>

class SpinMutex: public NonCopyable
{
public:
	SpinMutex(): lock_(1) {}
	inline bool tryAcquire() { return OSSpinLockTry(&lock_); }
	void acquire() { OSSpinLockLock(&lock_); }
	void release() { OSSpinLockUnlock(&lock_); }
private:
	OSSpinLock lock_;
};

#else // __APPLE__

#include <pthread.h>
#include "Exception.hpp"
#include "NonCopyable.hpp"

namespace pona
{

class SpinMutex: public NonCopyable
{
public:
	SpinMutex() {
		int ret = pthread_spin_init(&lock_, PTHREAD_PROCESS_PRIVATE);
		if (ret != 0)
			PONA_PTHREAD_EXCEPTION("pthread_spin_init", ret);
	}
	~SpinMutex() {
		int ret = pthread_spin_destroy(&lock_);
		if (ret != 0)
			PONA_PTHREAD_EXCEPTION("pthread_spin_destroy", ret);
	}
	inline bool tryAcquire() {
		int ret = pthread_spin_trylock(&lock_);
		if ((ret != 0) && (ret != EBUSY))
			PONA_PTHREAD_EXCEPTION("pthread_spin_trylock", ret);
		return ret != EBUSY;
	}
	inline void acquire() {
		int ret = pthread_spin_lock(&lock_);
		if (ret != 0)
			PONA_PTHREAD_EXCEPTION("pthread_spin_lock", ret);
	}
	inline void release() {
		int ret = pthread_spin_unlock(&lock_);
		if (ret != 0)
			PONA_PTHREAD_EXCEPTION("pthread_spin_unlock", ret);
	}
private:
	pthread_spinlock_t lock_;
};

#endif // __APPLE__

} // namespace pona

#endif // PONA_SPINMUTEX_HPP
