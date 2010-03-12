/*
 * GlobalCoreMutex.hpp -- process-vide spin mutex
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_GLOBALCOREMUTEX_HPP
#define PONA_GLOBALCOREMUTEX_HPP

#include "Instance.hpp"
#include "CoreMutex.hpp"
#include "Ref.hpp"

namespace pona
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

class GlobalCoreMutex: public Instance, public CoreMutex
{
public:
	static Ref<GlobalCoreMutex> instance();
	
private:
	GlobalCoreMutex() {}
};

inline Ref<GlobalCoreMutex> globalCoreMutex() { return GlobalCoreMutex::instance(); }

} // namespace pona

#endif // PONA_GLOBALCOREMUTEX_HPP
