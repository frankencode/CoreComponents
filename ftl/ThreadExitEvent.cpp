/*
 * ThreadExitEvent.cpp -- call event handlers on thread termination
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */

#include "ThreadExitEvent.hpp"

namespace ftl
{

ThreadExitEvent::ThreadExitEvent()
	: pid_(Process::currentId())
{}

ThreadExitEvent::~ThreadExitEvent()
{
	if (Process::currentId() == pid_) run();
}

} // namespace ftl
