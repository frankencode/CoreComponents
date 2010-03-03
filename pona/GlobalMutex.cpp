/*
 * GlobalMutex.cpp -- process-vide spin mutex
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../LICENSE for the license.
 */

#include "GlobalMutex.hpp"

namespace pona
{

int GlobalSpinMutexInitializer::count_ = 0;

GlobalSpinMutexInitializer::GlobalSpinMutexInitializer()
{
	if (count_ == 0) {
		++count_;
		GlobalMutex::instance();
	}
}

Ref<GlobalMutex> GlobalMutex::instance()
{
	static Ref<GlobalMutex, Owner> instance_ = 0;
	if (!instance_)
		instance_ = new GlobalMutex;
	return instance_;
}

} // namespace pona
