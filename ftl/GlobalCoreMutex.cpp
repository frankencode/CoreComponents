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

Ref<GlobalCoreMutex> GlobalCoreMutex::instance()
{
	static Ref<GlobalCoreMutex, Owner> instance_ = 0;
	if (!instance_)
		instance_ = new GlobalCoreMutex;
	return instance_;
}

} // namespace ftl
