 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */
#ifndef FTL_SPINLOCK_HPP
#define FTL_SPINLOCK_HPP

#include "Exception.hpp"

namespace ftl
{

class SpinLock
{
public:
	SpinLock(): flag_(0) {}

	inline bool tryAcquire() {
		return __sync_bool_compare_and_swap(&flag_, 0, 1);
	}
	inline void acquire() {
		while (!tryAcquire());
	}
	inline void release() {
		bool ok = __sync_bool_compare_and_swap(&flag_, 1, 0);
		(void)ok;
		FTL_ASSERT2(ok, "Double unlocking of a SpinLock");
	}
private:
	volatile char flag_;
};

} // namespace ftl

#endif // FTL_SPINLOCK_HPP
