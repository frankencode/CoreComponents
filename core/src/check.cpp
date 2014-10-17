/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <signal.h>
#include <flux/check>

namespace flux {

void abort()
{
	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGABRT);
	pthread_sigmask(SIG_UNBLOCK, &set, 0);
	raise(SIGABRT);
}

} // namespace flux
