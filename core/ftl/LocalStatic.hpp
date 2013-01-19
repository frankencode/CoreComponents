 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */
#ifndef FTL_LOCALSTATIC_HPP
#define FTL_LOCALSTATIC_HPP

#include "GlobalCoreMutex.hpp"
#include "Guard.hpp"

namespace ftl
{

/** Initialize a local static variable in a thread-safe manner
  */
template<class T, class Scope = None>
class LocalStatic
{
public:
	static T &instance()
	{
		Guard<SpinLock> guard(globalCoreMutex());
		static T instance_;
		return instance_;
	}
};

template<class T, class Scope>
inline T &localStatic() { return LocalStatic<T, Scope>::instance(); }

template<class T>
inline T &localStatic() { return LocalStatic<T, None>::instance(); }

} // namespace ftl

#endif // FTL_LOCALSTATIC_HPP
