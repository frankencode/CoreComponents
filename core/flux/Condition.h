/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_CONDITION_H
#define FLUX_CONDITION_H

#include "Mutex.h"

namespace flux
{

class Condition: public Object
{
public:
	inline static Ref<Condition> create() { return new Condition; }
	~Condition();
	void wait(Mutex *mutex);
	bool waitUntil(double timeout, Mutex *mutex);
	void signal();
	void broadcast();

private:
	Condition();

	pthread_cond_t cond_;
};

} // namespace flux

#endif // FLUX_CONDITION_H
