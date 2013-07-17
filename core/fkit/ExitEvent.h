/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FKIT_EXITEVENT_H
#define FKIT_EXITEVENT_H

#include "Singleton.h"
#include "Event.h"
#include "Process.h"

namespace fkit
{

class ExitEvent: public Event, public Singleton<ExitEvent>
{
protected:
	friend class Singleton<ExitEvent>;

	ExitEvent();
	~ExitEvent();

	pid_t pid_;
};

inline ExitEvent *exitEvent() { return ExitEvent::instance(); }

} // namespace fkit

#endif // FKIT_EXITEVENT_H
