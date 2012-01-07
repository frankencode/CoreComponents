/*
 * Condition.hpp -- wait condition
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */
#ifndef FTL_CONDITION_HPP
#define FTL_CONDITION_HPP

#include "atoms"
#include "Mutex.hpp"
#include "Time.hpp"

namespace ftl
{

class Condition: public Instance
{
public:
	Condition();
	~Condition();
	void wait(Mutex* mutex);
	bool waitUntil(Mutex* mutex, Time timeout);
	void signal();
	void broadcast();

private:
	pthread_cond_t cond_;
};

} // namespace ftl

#endif // FTL_CONDITION_HPP
