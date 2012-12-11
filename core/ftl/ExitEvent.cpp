/*
 * ExitEvent.cpp -- call event handlers on process termination
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
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
