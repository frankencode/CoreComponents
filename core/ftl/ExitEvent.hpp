 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */
#ifndef FTL_EXITEVENT_HPP
#define FTL_EXITEVENT_HPP

#include "atoms"
#include "Singleton.hpp"
#include "Event.hpp"
#include "Process.hpp"

namespace ftl
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

} // namespace ftl

#endif // FTL_EXITEVENT_HPP
