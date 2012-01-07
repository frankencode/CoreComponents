/*
 * ExitEvent.hpp -- call event handlers on process termination
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
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
