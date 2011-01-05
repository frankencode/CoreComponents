/*
 * ThreadExitEvent.hpp -- call event handlers on thread termination
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */
#ifndef FTL_THREADEXITEVENT_HPP
#define FTL_THREADEXITEVENT_HPP

#include "atoms"
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

inline Ref<ThreadExitEvent> threadExitEvent() { return ThreadExitEvent::instance(); }

} // namespace ftl

#endif // FTL_THREADEXITEVENT_HPP
