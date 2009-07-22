/*
 * ThreadExitEvent.hpp -- call event handlers on thread termination
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_THREADEXITEVENT_HPP
#define PONA_THREADEXITEVENT_HPP

#include "atom"
#include "ThreadLocalOwner.hpp"
#include "Event.hpp"
#include "Process.hpp"

namespace pona
{

class ThreadExitEvent: public Event
{
public:
	static Ref<ThreadExitEvent> instance();
	
private:
	ThreadExitEvent();
	~ThreadExitEvent();
	
	pid_t pid_;
};

inline Ref<ThreadExitEvent> threadExitEvent() { return ThreadExitEvent::instance(); }

} // namespace pona

#endif // PONA_THREADEXITEVENT_HPP
