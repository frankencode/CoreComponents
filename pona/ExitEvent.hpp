/*
 * ExitEvent.hpp -- call event handlers on process termination
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_EXITEVENT_HPP
#define PONA_EXITEVENT_HPP

#include "atoms"
#include "Mutex.hpp"
#include "Event.hpp"
#include "Process.hpp"

namespace pona
{

class ExitEvent: public Event
{
public:
	static Ref<ExitEvent> instance();
	
private:
	ExitEvent();
	~ExitEvent();
	
	pid_t pid_;
};

inline Ref<ExitEvent> exitEvent() { return ExitEvent::instance(); }

} // namespace pona

#endif // PONA_EXITEVENT_HPP
