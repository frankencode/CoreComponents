/*
 * ExitEvent.hpp -- call event handlers on process termination
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
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
private:
	friend class Singleton<ExitEvent>;
	
	ExitEvent();
	~ExitEvent();
	
	pid_t pid_;
};

inline Ref<ExitEvent> exitEvent() { return ExitEvent::instance(); }

} // namespace ftl

#endif // FTL_EXITEVENT_HPP
