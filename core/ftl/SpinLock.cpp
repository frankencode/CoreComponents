 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */

#include <pthread.h>
#include "SpinLock.hpp"

namespace ftl
{

void SpinLock::yield()
{
	int ret = pthread_yield();
	if (ret != 0)
		FTL_PTHREAD_EXCEPTION("pthread_yield", ret);
}

} // namespace ftl
