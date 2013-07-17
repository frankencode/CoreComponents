/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FKIT_GLOBALCOREMUTEX_H
#define FKIT_GLOBALCOREMUTEX_H

#include "Object.h"
#include "SpinLock.h"
#include "Ref.h"

namespace fkit
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

} // namespace fkit

#endif // FKIT_GLOBALCOREMUTEX_H
