/*
 * SpinLock.hpp -- a spinning mutex lock
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
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
		FTL_ASSERT2(ok, "Double unlocking of a SpinLock");
	}
private:
	volatile char flag_;
};

} // namespace ftl

#endif // FTL_SPINLOCK_HPP
