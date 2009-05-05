/*
 * ThreadExitEvent.cpp -- call event handlers on thread termination
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */

#include "ThreadExitEvent.hpp"

namespace pona
{

Ref<ThreadExitEvent> ThreadExitEvent::instance()
{
	static Ref<ThreadExitEvent, ThreadLocalOwner> instance_ = 0;
	
	if (!instance_)
		instance_ = new ThreadExitEvent;
	return instance_;
}

ThreadExitEvent::ThreadExitEvent()
	: pid_(pid())
{}

ThreadExitEvent::~ThreadExitEvent()
{
	if (pid() == pid_) run();
}

} // namespace pona
