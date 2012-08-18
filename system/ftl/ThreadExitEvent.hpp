/*
 * ThreadExitEvent.hpp -- call event handlers on thread termination
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
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
