 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
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
	inline static Ref<Condition, Owner> create() { return new Condition; }
	~Condition();
	void wait(Mutex *mutex);
	bool waitUntil(Mutex *mutex, Time timeout);
	void signal();
	void broadcast();

private:
	Condition();

	pthread_cond_t cond_;
};

} // namespace ftl

#endif // FTL_CONDITION_HPP
