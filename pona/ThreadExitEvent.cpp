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

ThreadExitEvent::ThreadExitEvent()
	: pid_(Process::currentProcessId())
{}

ThreadExitEvent::~ThreadExitEvent()
{
	if (Process::currentProcessId() == pid_) run();
}

} // namespace pona
