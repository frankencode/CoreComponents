/*
 * ThreadExitEvent.cpp -- call event handlers on thread termination
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
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
