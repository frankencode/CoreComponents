/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "GlobalCoreMutex.h"

namespace flux
{

int GlobalCoreMutexInitializer::count_ = 0;

GlobalCoreMutexInitializer::GlobalCoreMutexInitializer()
{
	if (count_ == 0) {
		++count_;
		GlobalCoreMutex::instance();
	}
}

GlobalCoreMutex *GlobalCoreMutex::instance()
{
	static GlobalCoreMutex instance_;
	return &instance_;
}

} // namespace flux
