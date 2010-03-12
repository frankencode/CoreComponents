/*
 * CoreMutex.hpp -- mutex optimized for minimal object size
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../LICENSE for the license.
 */

#ifndef PONA_COREMUTEX_HPP
#define PONA_COREMUTEX_HPP

#include <unistd.h> // _POSIX_SPIN_LOCKS, __APPLE__

#ifndef PONA_COREMUTEX_USE_POSIX_SEMAPHORES
	#ifndef PONA_COREMUTEX_USE_MACOS_SPIN_LOCKS
		#ifndef PONA_COREMUTEX_USE_POSIX_SEMAPHORES
			#ifdef _POSIX_SPIN_LOCKS
				#define PONA_COREMUTEX_USE_POSIX_SPIN_LOCKS
			#else
				#ifdef __APPLE__
					#define PONA_COREMUTEX_USE_MACOS_SPIN_LOCKS
				#else
					#define PONA_COREMUTEX_USE_POSIX_SEMAPHORES
				#endif
			#endif
		#endif
	#endif
#endif

#ifdef PONA_COREMUTEX_USE_POSIX_SPIN_LOCKS

#include <pthread.h>
#include "Exception.hpp"
#include "NonCopyable.hpp"

namespace pona
{

class CoreMutex: public NonCopyable
{
public:
	CoreMutex() {
		int ret = pthread_spin_init(&lock_, PTHREAD_PROCESS_PRIVATE);
		if (ret != 0)
			PONA_PTHREAD_EXCEPTION("pthread_spin_init", ret);
	}
	~CoreMutex() {
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

} // namespace pona

#endif // PONA_COREMUTEX_USE_POSIX_SPIN_LOCKS

#ifdef PONA_COREMUTEX_USE_MACOS_SPIN_LOCKS

#include <libkern/OSAtomic.h>

namespace pona
{

class CoreMutex: public NonCopyable
{
public:
	CoreMutex(): lock_(1) {}
	inline bool tryAcquire() { return OSSpinLockTry(&lock_); }
	void acquire() { OSSpinLockLock(&lock_); }
	void release() { OSSpinLockUnlock(&lock_); }
private:
	OSSpinLock lock_;
};

} // namespace pona

#endif // PONA_COREMUTEX_USE_MACOS_SPIN_LOCKS

#ifdef PONA_COREMUTEX_USE_POSIX_SEMAPHORES

#include <semaphore.h>
#include "Exception.hpp"
#include "NonCopyable.hpp"

namespace pona
{

class CoreMutex: public NonCopyable
{
public:
	CoreMutex() { if (sem_init(&sem_, 0/*pshared*/, 1/*value*/) == -1) PONA_SYSTEM_EXCEPTION; }
	~CoreMutex() { if (sem_destroy(&sem_) == -1) PONA_SYSTEM_EXCEPTION; }
	inline bool tryAcquire() {
		int ret = 0;
		while (true) {
			int ret = sem_trywait(&sem_);
			if (ret == -1) {
				if (errno == EINTR) continue;
				if (errno != EAGAIN)
					PONA_SYSTEM_EXCEPTION;
			}
			break;
		}
		return ret == 0;
	}
	void acquire() {
		while (true) {
			int ret = sem_wait(&sem_);
			if (ret == -1) {
				if (errno == EINTR) continue;
				PONA_SYSTEM_EXCEPTION;
			}
			break;
		}
	}
	void release() {
		if (sem_post(&sem_) == -1)
			PONA_SYSTEM_EXCEPTION;
	}
private:
	sem_t sem_;
};

} // namespace pona

#endif // PONA_COREMUTEX_USE_POSIX_SEMAPHORES

#endif // PONA_COREMUTEX_HPP
