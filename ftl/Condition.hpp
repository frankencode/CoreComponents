/*
 * Condition.hpp -- wait condition
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
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
