/*
 * debug.cpp -- low-level debugging helper
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */

#include <signal.h>
#include "debug.hpp"

namespace ftl
{

void abort()
{
	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGABRT);
	pthread_sigmask(SIG_UNBLOCK, &set, 0);
	raise(SIGABRT);
}

} // namespace ftl
