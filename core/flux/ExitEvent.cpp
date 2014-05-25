/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "Singleton.h"
#include "ExitEvent.h"

namespace flux
{

ExitEvent::ExitEvent()
	: pid_(Process::currentId())
{}

ExitEvent::~ExitEvent()
{
	if (Process::currentId() == pid_) run();
}

ExitEvent *exitEvent() { return Singleton<ExitEvent>::instance(); }

} // namespace flux
