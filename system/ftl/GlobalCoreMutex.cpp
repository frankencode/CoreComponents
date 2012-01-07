/*
 * GlobalCoreMutex.cpp -- process-vide spin mutex
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
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
