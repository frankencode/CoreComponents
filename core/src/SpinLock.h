/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUX_SPINLOCK_H
#define FLUX_SPINLOCK_H

namespace flux {

class SpinLock
{
public:
	SpinLock();
	~SpinLock();
	bool tryAcquire();
	void acquire();
	void release();

private:
	void yield();
	volatile char flag_;
};

#ifdef NDEBUG
inline SpinLock::SpinLock(): flag_(0) {}
inline SpinLock::~SpinLock() {}
inline bool SpinLock::tryAcquire() { return __sync_bool_compare_and_swap(&flag_, 0, 1); }
inline void SpinLock::acquire() { while (!__sync_bool_compare_and_swap(&flag_, 0, 1)) yield(); }
inline void SpinLock::release() { flag_ = 0; }
#endif // def NDEBUG

} // namespace flux

#endif // FLUX_SPINLOCK_H
