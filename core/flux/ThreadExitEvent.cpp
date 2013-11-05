/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "ThreadExitEvent.h"

namespace flux
{

ThreadExitEvent::ThreadExitEvent()
	: pid_(Process::currentId())
{}

ThreadExitEvent::~ThreadExitEvent()
{
	if (Process::currentId() == pid_) run();
}

} // namespace flux
