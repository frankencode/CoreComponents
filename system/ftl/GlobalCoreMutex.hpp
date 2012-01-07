/*
 * GlobalCoreMutex.hpp -- process-vide spin mutex
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */
#ifndef FTL_GLOBALCOREMUTEX_HPP
#define FTL_GLOBALCOREMUTEX_HPP

#include "Instance.hpp"
#include "SpinLock.hpp"
#include "Ref.hpp"

namespace ftl
{

class GlobalCoreMutex;

class GlobalCoreMutexInitializer
{
public:
	GlobalCoreMutexInitializer();
private:
	static int count_;
};

namespace { GlobalCoreMutexInitializer globalCoreMutexInitializer; }

class GlobalCoreMutex: public SpinLock
{
public:
	static GlobalCoreMutex* instance();
	
private:
	GlobalCoreMutex() {}
};

inline GlobalCoreMutex* globalCoreMutex() { return GlobalCoreMutex::instance(); }

} // namespace ftl

#endif // FTL_GLOBALCOREMUTEX_HPP
