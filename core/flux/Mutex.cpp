/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "Exception.h"
#include "Mutex.h"

namespace flux
{

Mutex::Mutex()
{
	pthread_mutexattr_t *pattr = 0;
	int ret;
	#ifndef NDEBUG
	pthread_mutexattr_t attr;
	pattr = &attr;
	ret = pthread_mutexattr_init(&attr);
	if (ret != 0)
		FLUX_PTHREAD_EXCEPTION("pthread_mutexattr_init", ret);
	ret = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
	if (ret != 0)
		FLUX_PTHREAD_EXCEPTION("pthread_mutexattr_settype", ret);
	#endif
	ret = pthread_mutex_init(&mutex_, pattr);
	if (ret != 0)
		FLUX_PTHREAD_EXCEPTION("pthread_mutex_init", ret);
	#ifndef NDEBUG
	ret = pthread_mutexattr_destroy(&attr);
	if (ret != 0)
		FLUX_PTHREAD_EXCEPTION("pthread_mutexattr_destroy", ret);
	#endif
}

Mutex::~Mutex()
{
	int ret = pthread_mutex_destroy(&mutex_);
	if (ret != 0)
		FLUX_PTHREAD_EXCEPTION("pthread_mutex_destroy", ret);
}

bool Mutex::tryAcquire()
{
	int ret = -1;
	while (true) {
		ret = pthread_mutex_trylock(&mutex_);
		if (ret != EINTR) break;
	}
	if ((ret != 0) && (ret != EBUSY))
		FLUX_PTHREAD_EXCEPTION("pthread_mutex_trylock", ret);
	return ret != EBUSY;
}

void Mutex::acquire()
{
	int ret = -1;
	while (true) {
		ret = pthread_mutex_lock(&mutex_);
		if (ret != EINTR) break;
	}
	if (ret != 0)
		FLUX_PTHREAD_EXCEPTION("pthread_mutex_lock", ret);
}

void Mutex::release()
{
	int ret = pthread_mutex_unlock(&mutex_);
	if (ret != 0)
		FLUX_PTHREAD_EXCEPTION("pthread_mutex_unlock", ret);
}

} // namespace flux
