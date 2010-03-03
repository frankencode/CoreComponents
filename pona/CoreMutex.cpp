/*
 * CoreMutex.cpp -- non-recursive mutex lock
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../LICENSE for the license.
 */

#if 0 // __GNUC__ > 3

#else // __GNUC__ > 3

#include <pthread.h>
#ifndef EBUSY
#include <errno.h>
#endif
#include "Exception.hpp"
#include "CoreMutex.hpp"

namespace pona
{

CoreMutex::CoreMutex()
{
	pthread_mutexattr_t* pattr = 0;
	int ret;
	#ifndef NDEBUG
	pthread_mutexattr_t attr;
	pattr = &attr;
	ret = pthread_mutexattr_init(&attr);
	if (ret != 0)
		PONA_PTHREAD_EXCEPTION("pthread_mutexattr_init", ret);
	ret = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
	if (ret != 0)
		PONA_PTHREAD_EXCEPTION("pthread_mutexattr_settype", ret);
	#endif
	ret = pthread_mutex_init(&mutex_, pattr);
	if (ret != 0)
		PONA_PTHREAD_EXCEPTION("pthread_mutex_init", ret);
	#ifndef NDEBUG
	ret = pthread_mutexattr_destroy(&attr);
	if (ret != 0)
		PONA_PTHREAD_EXCEPTION("pthread_mutexattr_destroy", ret);
	#endif
}

CoreMutex::~CoreMutex()
{
	int ret = pthread_mutex_destroy(&mutex_);
	if (ret != 0)
		PONA_PTHREAD_EXCEPTION("pthread_mutex_destroy", ret);
}

bool CoreMutex::tryAcquire()
{
	int ret = -1;
	while (true) {
		ret = pthread_mutex_trylock(&mutex_);
		if (ret != EINTR) break;
	}
	if ((ret != 0) && (ret != EBUSY))
		PONA_PTHREAD_EXCEPTION("pthread_mutex_trylock", ret);
	return ret != EBUSY;
}

void CoreMutex::acquire()
{
	int ret = -1;
	while (true) {
		ret = pthread_mutex_lock(&mutex_);
		if (ret != EINTR) break;
	}
	if (ret != 0)
		PONA_PTHREAD_EXCEPTION("pthread_mutex_lock", ret);
}

void CoreMutex::release()
{
	int ret = pthread_mutex_unlock(&mutex_);
	if (ret != 0)
		PONA_PTHREAD_EXCEPTION("pthread_mutex_unlock", ret);
}

} // namespace pona

#endif // __GNUC__ > 3
