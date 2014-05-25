/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_EXITEVENT_H
#define FLUX_EXITEVENT_H

#include "Event.h"
#include "Process.h"

namespace flux
{

template<class SubClass> class Singleton;

class ExitEvent: public Event
{
protected:
	friend class Singleton<ExitEvent>;

	ExitEvent();
	~ExitEvent();

	pid_t pid_;
};

ExitEvent *exitEvent();

} // namespace flux

#endif // FLUX_EXITEVENT_H
