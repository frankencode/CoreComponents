/*
 * LocalStatic -- allocation helper for local static variables
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */
#ifndef FTL_LOCALSTATIC_HPP
#define FTL_LOCALSTATIC_HPP

#include "GlobalCoreMutex.hpp"
#include "Mutex.hpp"
#include "ScopeGuard.hpp"
#include "Guard.hpp"

namespace ftl
{

/** Initialize a local static variable in a thread-safe manner
  * (without requiring static local initialization to be supported by the compiler).
  * Related thought: Maybe the compiler should warn about local statics instead of
  * automatically throwing in fancy locking logic.
  */
template<class T, class Scope = None>
class LocalStatic
{
public:
	static T& instance()
	{
		Guard<CoreMutex> guard1(globalCoreMutex());
		static Mutex mutex_;
		guard1.dismiss();
		ScopeGuard<Mutex> guard2(&mutex_);
		static T instance_;
		return instance_;
	}
};

template<class T, class Scope>
inline T& localStatic() { return LocalStatic<T, Scope>::instance(); }

template<class T>
inline T& localStatic() { return LocalStatic<T, None>::instance(); }

} // namespace ftl

#endif // FTL_LOCALSTATIC_HPP
