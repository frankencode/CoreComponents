/*
 * ExitEvent.cpp -- call event handlers on process termination
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */

#include "ExitEvent.hpp"

namespace ftl
{

ExitEvent::ExitEvent()
	: pid_(Process::currentId())
{}

ExitEvent::~ExitEvent()
{
	if (Process::currentId() == pid_) run();
}

} // namespace ftl
