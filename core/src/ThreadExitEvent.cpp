/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/ThreadExitEvent>

namespace flux {

ThreadExitEvent::ThreadExitEvent()
	: pid_(Process::currentId())
{}

ThreadExitEvent::~ThreadExitEvent()
{
	if (Process::currentId() == pid_) run();
}

} // namespace flux
