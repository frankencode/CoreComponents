 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
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
