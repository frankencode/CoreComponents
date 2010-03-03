/*
 * GlobalMutex.hpp -- process-vide spin mutex
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_GLOBALMUTEX_HPP
#define PONA_GLOBALMUTEX_HPP

#include "Instance.hpp"
#include "CoreMutex.hpp"
#include "Ref.hpp"

namespace pona
{

class GlobalMutex;

class GlobalSpinMutexInitializer
{
public:
	GlobalSpinMutexInitializer();
private:
	static int count_;
};

namespace { GlobalSpinMutexInitializer globalSpinMutexInitializer; }

class GlobalMutex: public Instance, public CoreMutex
{
public:
	static Ref<GlobalMutex> instance();
	
private:
	GlobalMutex() {}
};

inline Ref<GlobalMutex> globalSpinMutex() { return GlobalMutex::instance(); }

} // namespace pona

#endif // PONA_GLOBALMUTEX_HPP
