 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */
#ifndef FTL_THREADEXITEVENT_HPP
#define FTL_THREADEXITEVENT_HPP

#include "ThreadLocalSingleton.hpp"
#include "Event.hpp"
#include "Process.hpp"

namespace ftl
{

class ThreadExitEvent: public Event, public ThreadLocalSingleton<ThreadExitEvent>
{
private:
	friend class ThreadLocalSingleton<ThreadExitEvent>;

	ThreadExitEvent();
	~ThreadExitEvent();

	pid_t pid_;
};

inline ThreadExitEvent *threadExitEvent() { return ThreadExitEvent::instance(); }

} // namespace ftl

#endif // FTL_THREADEXITEVENT_HPP
