/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FKIT_SPINLOCK_H
#define FKIT_SPINLOCK_H

#include "Exception.h"

namespace fkit
{

class SpinLock
{
public:
	SpinLock(): flag_(0) {}

	inline bool tryAcquire() {
		return __sync_bool_compare_and_swap(&flag_, 0, 1);
	}
	inline void acquire() {
		while (!tryAcquire()) yield();
	}
	inline void release() {
		flag_ = 0;
	}

private:
	void yield();
	volatile char flag_;
};

} // namespace fkit

#endif // FKIT_SPINLOCK_H
