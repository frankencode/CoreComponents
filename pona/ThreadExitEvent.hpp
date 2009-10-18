/*
 * ThreadExitEvent.hpp -- call event handlers on thread termination
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_THREADEXITEVENT_HPP
#define PONA_THREADEXITEVENT_HPP

#include "atoms"
#include "ThreadLocalOwner.hpp"
#include "Event.hpp"
#include "Process.hpp"

namespace pona
{

class PONA_API ThreadExitEvent: public Event
{
public:
	static Ref<ThreadExitEvent> instance();
	
private:
	PONA_INTERN ThreadExitEvent();
	PONA_INTERN ~ThreadExitEvent();
	
	pid_t pid_;
};

inline Ref<ThreadExitEvent> threadExitEvent() { return ThreadExitEvent::instance(); }

} // namespace pona

#endif // PONA_THREADEXITEVENT_HPP
