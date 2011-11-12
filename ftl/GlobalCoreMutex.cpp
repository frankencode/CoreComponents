/*
 * GlobalCoreMutex.cpp -- process-vide spin mutex
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */

#include "GlobalCoreMutex.hpp"

namespace ftl
{

int GlobalCoreMutexInitializer::count_ = 0;

GlobalCoreMutexInitializer::GlobalCoreMutexInitializer()
{
	if (count_ == 0) {
		++count_;
		GlobalCoreMutex::instance();
	}
}

GlobalCoreMutex* GlobalCoreMutex::instance()
{
	static GlobalCoreMutex instance_;
	return &instance_;
}

} // namespace ftl
