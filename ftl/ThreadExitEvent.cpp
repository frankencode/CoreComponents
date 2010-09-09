/*
 * ThreadExitEvent.cpp -- call event handlers on thread termination
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../COPYING for the license.
 */

#include "ThreadExitEvent.hpp"

namespace ftl
{

ThreadExitEvent::ThreadExitEvent()
	: pid_(Process::currentProcessId())
{}

ThreadExitEvent::~ThreadExitEvent()
{
	if (Process::currentProcessId() == pid_) run();
}

} // namespace ftl
