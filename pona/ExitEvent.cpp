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

ExitEvent::ExitEvent()
	: pid_(Process::currentProcessId())
{}

ExitEvent::~ExitEvent()
{
	if (Process::currentProcessId() == pid_) run();
}

} // namespace pona
