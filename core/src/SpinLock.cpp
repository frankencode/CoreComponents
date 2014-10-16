/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <pthread.h>
#include "SpinLock"

namespace flux
{

void SpinLock::yield()
{
	pthread_yield();
}

} // namespace flux
