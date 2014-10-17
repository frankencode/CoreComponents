/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <pthread.h>
#include <flux/SpinLock>
#include "helgrind.h"

namespace flux {

#ifndef NDEBUG
SpinLock::SpinLock():
	flag_(0)
{
	VALGRIND_HG_MUTEX_INIT_POST(&flag_, 0);
}

SpinLock::~SpinLock()
{
	VALGRIND_HG_MUTEX_DESTROY_PRE(&flag_);
}

bool SpinLock::tryAcquire()
{
	VALGRIND_HG_MUTEX_LOCK_PRE(&flag_, 1);
	bool ok = __sync_bool_compare_and_swap(&flag_, 0, 1);
	VALGRIND_HG_MUTEX_LOCK_POST(&flag_);
	return ok;
}

void SpinLock::acquire()
{
	VALGRIND_HG_MUTEX_LOCK_PRE(&flag_, 0);
	while (!__sync_bool_compare_and_swap(&flag_, 0, 1)) yield();
	VALGRIND_HG_MUTEX_LOCK_POST(&flag_);
}

void SpinLock::release()
{
	VALGRIND_HG_MUTEX_UNLOCK_PRE(&flag_);
	__sync_bool_compare_and_swap(&flag_, 1, 0);
	VALGRIND_HG_MUTEX_UNLOCK_POST(&flag_);
}
#endif // ndef NDEBUG

void SpinLock::yield()
{
	pthread_yield();
}

} // namespace flux
