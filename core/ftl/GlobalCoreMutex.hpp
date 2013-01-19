 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
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
	static GlobalCoreMutex *instance();

private:
	GlobalCoreMutex() {}
};

inline GlobalCoreMutex *globalCoreMutex() { return GlobalCoreMutex::instance(); }

} // namespace ftl

#endif // FTL_GLOBALCOREMUTEX_HPP
