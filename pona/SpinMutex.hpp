/*
 * SpinMutex.hpp -- recursive spinning mutex lock
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_SPINMUTEX_HPP
#define PONA_SPINMUTEX_HPP

#if __GNUC__ > 3

#include "types.hpp"

namespace pona
{

class SpinMutex
{
public:
	SpinMutex(): flag_(0) {}
	
	inline bool tryAcquire() {
		return __sync_bool_compare_and_swap(&flag_, 0, 1);
	}
	
	inline void acquire() {
		while (!tryAcquire()) {
			// Step back for an active wait on the local cache.
			// Give it a chance to not utilize 100% of read/write memory bandwidth
			// by readonly iddling on a local register, cached value or memory value.
			// (Depends on what the compiler loves to generate of this loop body.)
			// If the cache is shared with the other thread holding the lock,
			// the loop may return early.
			for (int i = 0; i < 16; ++i)
				if (i & 3 == 0)
					if (flag_ == 1) break;
			
			// We could also actively yield at this point using sched_yield(2).
			// But with a standard time slice scheduler there is a likelihood
			// that a yielded thread becomes run earlier than a thread which has
			// already fully consumed its slice.
			// And most likely the thread holding the lock will be woken up last.
			// Therefore the performance impact of the mutex would be difficult
			// to manage.
			// Note: On legacy systems without support for preemptive scheduling
			// we would have to use sched_yield(2).
		}
	}
	
	inline void release() {
		flag_ = 0;
	}
	
private:
	uint8_t flag_;
};

} // namespace pona

#else // __GNUC__ > 3

#include "CoreMutex.hpp"

namespace pona
{

typedef CoreMutex SpinMutex;

} // namespace pona

#endif // __GNUC__ > 3

#endif // PONA_SPINMUTEX_HPP
