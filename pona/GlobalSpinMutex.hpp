/*
 * GlobalSpinMutex.hpp -- process-vide spin mutex
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_GLOBALMUTEX_HPP
#define PONA_GLOBALMUTEX_HPP

#include "Instance.hpp"
#include "SpinMutex.hpp"
#include "Ref.hpp"

namespace pona
{

class GlobalSpinMutex;

class GlobalSpinMutexInitializer
{
public:
	GlobalSpinMutexInitializer();
private:
	static int count_;
};

namespace { GlobalSpinMutexInitializer globalSpinMutexInitializer; }

class GlobalSpinMutex: public Instance, public SpinMutex
{
public:
	static Ref<GlobalSpinMutex> instance();
	
private:
	GlobalSpinMutex() {}
};

inline Ref<GlobalSpinMutex> globalSpinMutex() { return GlobalSpinMutex::instance(); }

} // namespace pona

#endif // PONA_GLOBALMUTEX_HPP
