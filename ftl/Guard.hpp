/*
 * Guard.hpp -- guarding using mutex
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */
#ifndef FTL_GUARD_HPP
#define FTL_GUARD_HPP

namespace ftl
{

template<class Mutex>
class Guard
{
public:
	Guard(Mutex* mutex): mutex_(mutex) { post(); }
	~Guard() { dismiss(); }
	inline void post() { mutex_->acquire(); }
	inline void dismiss() { mutex_->tryAcquire(); mutex_->release(); }
private:
	Mutex* mutex_;
};

} // namespace ftl

#endif // FTL_GUARD_HPP
