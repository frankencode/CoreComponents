/*
 * ExitEvent.cpp -- call event handlers on process termination
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */

#include "ExitEvent.hpp"

namespace pona
{

Ref<ExitEvent> ExitEvent::instance()
{
	static Mutex mutex_;
	mutex_.acquire();
	static Ref<ExitEvent, Owner> instance_ = 0;
	if (!instance_)
		instance_ = new ExitEvent;
	mutex_.release();
	return instance_;
}

ExitEvent::ExitEvent()
	: pid_(Process::currentProcessId())
{}

ExitEvent::~ExitEvent()
{
	if (Process::currentProcessId() == pid_) run();
}

} // namespace pona
