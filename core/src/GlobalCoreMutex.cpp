/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include "GlobalCoreMutex"

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
