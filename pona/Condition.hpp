/*
 * Condition.hpp -- wait condition
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_CONDITION_HPP
#define PONA_CONDITION_HPP

#include <stdio.h>
#include <errno.h>

#include "atoms"
#include "Mutex.hpp"
#include "Time.hpp"

namespace pona
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

} // namespace pona

#endif // PONA_CONDITION_HPP
