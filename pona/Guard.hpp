/*
 * Guard.hpp -- guarding using mutex
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_GUARD_HPP
#define PONA_GUARD_HPP

namespace pona
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

} // namespace pona

#endif // PONA_GUARD_HPP
