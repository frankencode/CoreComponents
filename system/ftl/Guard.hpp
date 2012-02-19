/*
 * Guard.hpp -- guarding using mutex
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */
#ifndef FTL_GUARD_HPP
#define FTL_GUARD_HPP

namespace ftl
{

template<class Mutex>
class Guard
{
public:
	Guard(Mutex* mutex): mutex_(mutex) { mutex_->acquire(); }
	~Guard() { mutex_->release(); }
private:
	Mutex* mutex_;
};

} // namespace ftl

#endif // FTL_GUARD_HPP
