#ifndef PONA_SPINMUTEX_HPP
#define PONA_SPINMUTEX_HPP

#include <assert.h>
#include "defaults.hpp"
#include "visibility.hpp"

namespace pona
{

class PONA_API SpinMutex
{
public:
	SpinMutex(): flag_(0) {}
	
	inline bool tryAcquire() {
		return __sync_bool_compare_and_swap(&flag_, 0, 1);
	}
	
	inline void acquire() {
		while (!tryAcquire())
			threadYield();
	}
	
	inline void release() {
		flag_ = 0;
	}
	
private:
	static void threadYield();
	
	int flag_;
};

} // namespace pona

#endif // PONA_SPINMUTEX_HPP

